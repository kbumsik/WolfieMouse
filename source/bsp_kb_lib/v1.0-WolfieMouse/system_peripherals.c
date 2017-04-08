/*
 * peripherals.c
 *
 *  Created on: Mar 19, 2017
 *      Author: kbumsik
 */

#include "system_config.h"
#include "kb_common_source.h"
#include "system_control.h"

#include "kb_gpio.h"
#include "kb_terminal.h"
#include "kb_TCA9545A_i2c_mux.h"
#include "kb_VL6180X_range_finder.h"
#include "kb_HCMS-290X_display.h"
#include "encoder.h"
#include "motor.h"
#include "pid.h"
#include "kb_adc.h"

/*******************************************************************************
 * Global variables
 ******************************************************************************/
/**
 * Range finder value
 * Range_front is the average of range_front_left and range_front_right.
 */
volatile uint16_t range_L = 4095, range_R = 4095, range_F = 4095,
            range_FL = 4095, range_FR = 4095;

// Encoder value
volatile int32_t steps_L, steps_R, steps_L_old, steps_R_old;
volatile int32_t speed_L, speed_R, speed_D;

// PID handler
pid_handler_t pid_T;    // Translational
pid_handler_t pid_R; // Rotational

// ADC objects
#ifdef KB_BLACKWOLF
    kb_adc_t adc_L;
    kb_adc_t adc_R;
    kb_adc_t adc_F; // (Actually FL)
#endif

/*******************************************************************************
 * Static variables
 ******************************************************************************/
static volatile int is_peripherals_initialized = 0;
static volatile int is_pid_running = 0;
static volatile int is_pid_T_running = 0;
static volatile int is_pid_R_running = 0;
static volatile int is_range_running = 0;

/*******************************************************************************
 * Function Definition
 ******************************************************************************/
void peripheral_init(void)
{
    kb_gpio_init_t GPIO_InitStruct;

    /* Init motor */
    motor_init();
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);

    /* Configure UART */
    kb_terminal_init();

    /* Configure Pushbuttons */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    kb_gpio_init(B1_PORT, B1_PIN, &GPIO_InitStruct);
    kb_gpio_init(B2_PORT, B2_PIN, &GPIO_InitStruct);

    /* Configure LED pins */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    kb_gpio_init(LED1_PORT, LED1_PIN, &GPIO_InitStruct);
    kb_gpio_init(LED2_PORT, LED2_PIN, &GPIO_InitStruct);
    kb_gpio_init(LED3_PORT, LED3_PIN, &GPIO_InitStruct);
    kb_gpio_init(LED4_PORT, LED4_PIN, &GPIO_InitStruct);
#if defined(KB_BLACKWOLF)
    kb_gpio_init(LED5_PORT, LED5_PIN, &GPIO_InitStruct);
    kb_gpio_init(LED6_PORT, LED6_PIN, &GPIO_InitStruct);
#endif

    /* Configure LED Pins Output Level */
    kb_gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    kb_gpio_set(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    kb_gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_RESET);
    kb_gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_RESET);
#if defined(KB_BLACKWOLF)
    kb_gpio_set(LED5_PORT, LED5_PIN, GPIO_PIN_RESET);
    kb_gpio_set(LED6_PORT, LED6_PIN, GPIO_PIN_RESET);
#endif

    /* Init peripherals for the LED display */
    hcms_290x_init();

    /* Range Sensor */
#if defined(KB_WOLFIEMOUSE)
    /* Init i2c MUX */
    //tca9545a_init();
    //tca9545a_select_ch(TCA9545A_CH_0);

    /* Init Digital range sensors */
    //vl6180x_init();
    //vl6180x_range_mm();
#elif defined(KB_BLACKWOLF)
    /* ADC */
    // Left
    kb_adc_init_t adc_init;
    adc_init.device = KB_ADC1;
    adc_init.channel = KB_ADC_CH12;

    kb_adc_init(&adc_L, &adc_init);
    kb_adc_pin(RECV_L_PORT, RECV_L_PIN);
    // Right
    adc_init.device = KB_ADC2;
    adc_init.channel = KB_ADC_CH11;

    kb_adc_init(&adc_R, &adc_init);
    kb_adc_pin(RECV_R_PORT, RECV_R_PIN);
    // Front (actually FL)
    adc_init.device = KB_ADC3;
    adc_init.channel = KB_ADC_CH13;

    kb_adc_init(&adc_F, &adc_init);
    kb_adc_pin(RECV_FL_PORT, RECV_FL_PIN);

    /* Emitter */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    kb_gpio_init(EMITTER_SIDES_PORT, EMITTER_SIDES_PIN, &GPIO_InitStruct);
    kb_gpio_init(EMITTER_FL_PORT, EMITTER_FL_PIN, &GPIO_InitStruct);

    kb_gpio_set(EMITTER_SIDES_PORT, EMITTER_SIDES_PIN, GPIO_PIN_RESET);
    kb_gpio_set(EMITTER_FL_PORT, EMITTER_FL_PIN, GPIO_PIN_RESET);
#else
    #error "Define one of WolfieMouse hardware"
#endif

    /* Init encoder */
    encoder_init();
    steps_L = encoder_left_count();
    steps_R = encoder_right_count();

    /* Set PID controller */
    pid_value_t pid_T_value = {
            .kp = 0.9,
            .ki = 0,
            .kd = 0.1
    };

    pid_value_t pid_R_value = {
                .kp = 0.2,
                .ki = 0,
                .kd = 0.05
    };

    pid_set_pid(&pid_T, &pid_T_value);
    pid_reset(&pid_T);

    pid_set_pid(&pid_R, &pid_R_value);
    pid_reset(&pid_R);

    pid_input_setpoint(&pid_T, 0);
    pid_input_setpoint(&pid_R, 0);

    /* Now peripherals has been initialized */
    is_peripherals_initialized = 1;
}

/**
 * This overrides original SysTick_hook() located in kb_hooks.c
 * This function is automatically called every 1ms, triggered by SysTick.
 */
#include "config_measurements.h"

void SysTick_hook(void)
{
    if (!is_peripherals_initialized) {
        return;
    }
    // Get encoder values and calculate speed
    steps_L_old = steps_L;
    steps_R_old = steps_R;
    steps_L = encoder_left_count();
    steps_R = encoder_right_count();
    // FIXME: We double this because left side encoder 2 times slower
    speed_L = (steps_L - steps_L_old);// * CONFIG_LEN_PER_CNT;
    speed_R = (steps_R - steps_R_old);// * CONFIG_LEN_PER_CNT;
    speed_D = speed_L - speed_R;

    /* Start PID calculation */
    // calculate errorT
    int32_t feedback_T = (speed_L + speed_R) / 2;
    int32_t outputT = pid_compute(&pid_T, feedback_T);

    // Get ADC values
    if(is_range_running) {
        kb_gpio_set(EMITTER_SIDES_PORT, EMITTER_SIDES_PIN, GPIO_PIN_SET);
        kb_delay_us(60);
        range_L = kb_adc_measure(&adc_L);
        range_R = kb_adc_measure(&adc_R);
        // range front too
        kb_gpio_set(EMITTER_FL_PORT, EMITTER_FL_PIN, GPIO_PIN_SET);
        kb_delay_us(60);
        range_F = kb_adc_measure(&adc_F);
        kb_gpio_set(EMITTER_FL_PORT, EMITTER_FL_PIN, GPIO_PIN_RESET);
    }

    // get errorR
    int32_t feedback_R;
    if(is_range_running && (range_L > MEASURE_RANGE_L_DETECT)
                        && (range_R > MEASURE_RANGE_R_DETECT) ) {
        // If both range are within wall detecting distance,
        // Use range sensor to get rotational error
        feedback_R = (range_R - range_L - MEASURE_RANGE_R_OFFSET) / 10;
    } else if (is_range_running && range_L > MEASURE_RANGE_L_DETECT) {
        // If only left side is within range
        // use the middle value of the right range
        feedback_R = (MEASURE_RANGE_R_MIDDLE - range_L) / 10;
    } else if (is_range_running && range_R > MEASURE_RANGE_R_DETECT) {
        // If only right side is within range
        // use the middle value of the left range
        feedback_R = (range_R - MEASURE_RANGE_L_MIDDLE) / 10;
    } else {
        // in open space, use rotary encoder
        feedback_R = speed_L - speed_R;
    }

    int32_t outputR = pid_compute(&pid_R, feedback_R);

    if (!is_pid_T_running) {
        outputT = 0;
    }
    if (!is_pid_R_running) {
        outputR = 0;
    }
    if (!is_pid_running) {
        return;
    }

    // Apply to the motor
    motor_speed_percent(CH_LEFT, outputT + outputR);
    motor_speed_percent(CH_RIGHT, outputT - outputR);
    motor_start(CH_BOTH);
}


/*******************************************************************************
 * system functions from @system_control.h
 ******************************************************************************/
// Motor driving
void system_start_driving(void)
{
    is_pid_running = 1;
    is_pid_T_running = 1;
    is_pid_R_running = 1;
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);
}

void system_stop_driving(void)
{
    is_pid_running = 0;
    is_pid_T_running = 0;
    is_pid_R_running = 0;
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);
}

// Range finder control
void system_enable_range_finder(void)
{
    is_range_running = 1;
}

void system_disable_range_finder(void)
{
    is_range_running = 0;
}

void system_reset_encoder(void)
{
    encoder_left_reset();
    encoder_right_reset();
}
