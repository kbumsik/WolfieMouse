/*
 * peripherals.c
 *
 *  Created on: Mar 19, 2017
 *      Author: kbumsik
 */

#include "system_config.h"
#include "common_source.h"
#include "system_control.h"

#include "gpio.h"
#include "terminal.h"
#include "hcms_290x_display.h"
#include "encoder.h"
#include "motor.h"
#include "pid.h"
#include "adc.h"
#include "tick.h"

#include "FreeRTOS.h"
#include "semphr.h"

/*******************************************************************************
 * Global variables import
 ******************************************************************************/
extern uint32_t g_motion_target_L;
extern uint32_t g_motion_target_R;
extern SemaphoreHandle_t g_motion_semphr;

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
volatile struct encoder_data step, step_old;
volatile struct _speed_struct {
    int32_t left;
    int32_t right;
    int32_t diff;    // left - right
} speed;

// PID handler
pid_handler_t g_pid_T;    // Translational
pid_handler_t g_pid_R; // Rotational

// ADC objects
adc_t adc_L;
adc_t adc_R;
adc_t adc_FL; // (Actually FL)

/*******************************************************************************
 * Static variables
 ******************************************************************************/
static volatile int _peripherals_initialized = 0;
static volatile int _pid_running = 0;
static volatile int _pid_T_running = 0;
static volatile int _pid_R_running = 0;
static volatile int _range_running = 0;
enum motion_status {
    disabled, forward, backward
};
static enum motion_status _motion_status_L = disabled;
static enum motion_status _motion_status_R = disabled;

/*******************************************************************************
 * Function Definition
 ******************************************************************************/
void peripheral_init(void)
{
    gpio_init_t GPIO_InitStruct;

    /* Init motor */
    motor_init();
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);

    /* Configure UART */
    terminal_init();

    /* Configure Pushbuttons */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init(B1_PORT, B1_PIN, &GPIO_InitStruct);
    gpio_init(B2_PORT, B2_PIN, &GPIO_InitStruct);

    /* Configure LED pins */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init(LED1_PORT, LED1_PIN, &GPIO_InitStruct);
    gpio_init(LED2_PORT, LED2_PIN, &GPIO_InitStruct);
    gpio_init(LED3_PORT, LED3_PIN, &GPIO_InitStruct);
    gpio_init(LED4_PORT, LED4_PIN, &GPIO_InitStruct);
    gpio_init(LED5_PORT, LED5_PIN, &GPIO_InitStruct);
    gpio_init(LED6_PORT, LED6_PIN, &GPIO_InitStruct);

    /* Configure LED Pins Output Level */
    gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    gpio_set(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_RESET);
    gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_RESET);
    gpio_set(LED5_PORT, LED5_PIN, GPIO_PIN_RESET);
    gpio_set(LED6_PORT, LED6_PIN, GPIO_PIN_RESET);

    /* Init peripherals for the LED display */
    hcms_290x_init();

    /* Range Sensor */
    /* ADC */
    // Left
    adc_init_t adc_init_obj;
    adc_init_obj.device = KB_ADC1;
    adc_init_obj.channel = KB_ADC_CH12;

    adc_init(&adc_L, &adc_init_obj);
    adc_pin(RECV_L_PORT, RECV_L_PIN);
    // Right
    adc_init_obj.device = KB_ADC2;
    adc_init_obj.channel = KB_ADC_CH11;

    adc_init(&adc_R, &adc_init_obj);
    adc_pin(RECV_R_PORT, RECV_R_PIN);
    // Front (actually FL)
    adc_init_obj.device = KB_ADC3;
    adc_init_obj.channel = KB_ADC_CH13;

    adc_init(&adc_FL, &adc_init_obj);
    adc_pin(RECV_FL_PORT, RECV_FL_PIN);

    /* Emitter */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init(EMITTER_L_PORT, EMITTER_L_PIN, &GPIO_InitStruct);
    gpio_init(EMITTER_R_PORT, EMITTER_R_PIN, &GPIO_InitStruct);
    gpio_init(EMITTER_FL_PORT, EMITTER_FL_PIN, &GPIO_InitStruct);

    gpio_set(EMITTER_L_PORT, EMITTER_L_PIN, GPIO_PIN_RESET);
    gpio_set(EMITTER_R_PORT, EMITTER_R_PIN, GPIO_PIN_RESET);
    gpio_set(EMITTER_FL_PORT, EMITTER_FL_PIN, GPIO_PIN_RESET);

    /* Init encoder */
    encoder_init();
    encoder_get(&step, ENCODER_CH_BOTH);

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

    pid_set_pid(&g_pid_T, &pid_T_value);
    pid_reset(&g_pid_T);

    pid_set_pid(&g_pid_R, &pid_R_value);
    pid_reset(&g_pid_R);

    pid_input_setpoint(&g_pid_T, 0);
    pid_input_setpoint(&g_pid_R, 0);

    /* Now peripherals has been initialized */
    _peripherals_initialized = 1;
}

/**
 * This overrides original SysTick_hook() located in hooks.c
 * This function is automatically called every 1ms, triggered by SysTick.
 */
#include "config_measurements.h"

void SysTick_hook(void)
{
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (!_peripherals_initialized) {
        return;
    }
    // Get encoder values and calculate speed
    step_old = step;
    encoder_get(&step, ENCODER_CH_BOTH);

    speed.left = (step.left - step_old.left);// * CONFIG_LEN_PER_CNT;
    speed.right = (step.right - step_old.right);// * CONFIG_LEN_PER_CNT;
    speed.diff = speed.left - speed.right;

    /* Motion controller notification */
    for (int i = 0; i < 2; i++) {
        // Change variables being checked
        static enum motion_status *status;
        static int32_t steps;
        static uint32_t target;
        if (i == 0) {
            status = &_motion_status_L;
            steps = step.left;
            target = g_motion_target_L;
        } else {
            status = &_motion_status_R;
            steps = step.right;
            target = g_motion_target_R;
        }
        // check variables
        if (disabled != *status) {
            if ((forward == *status) &&
                    (steps >= target)) {
                *status = disabled;
                xSemaphoreGiveFromISR(g_motion_semphr, &xHigherPriorityTaskWoken);
            } else if ((backward == *status) &&
                    (steps <= target)) {
                *status = disabled;
                xSemaphoreGiveFromISR(g_motion_semphr, &xHigherPriorityTaskWoken);
            }
        }
    }

    /* Start PID calculation */
    // calculate errorT
    int32_t feedback_T = (speed.left + speed.right) / 2;
    int32_t outputT = pid_compute(&g_pid_T, feedback_T);

    // Get ADC values
    if(_range_running) {
        // Left
        gpio_set(EMITTER_L_PORT, EMITTER_L_PIN, GPIO_PIN_SET);
        delay_us(60);
        range_L = adc_measure(&adc_L);
        // Right
        gpio_set(EMITTER_R_PORT, EMITTER_R_PIN, GPIO_PIN_SET);
        delay_us(60);
        range_R = adc_measure(&adc_R);
        // range front too
        gpio_set(EMITTER_FL_PORT, EMITTER_FL_PIN, GPIO_PIN_SET);
        delay_us(60);
        range_F = adc_measure(&adc_FL);
        gpio_set(EMITTER_FL_PORT, EMITTER_FL_PIN, GPIO_PIN_RESET);
    }

    // get errorR
    int32_t feedback_R;
    if(_range_running && (range_L > MEASURE_RANGE_L_DETECT)
                        && (range_R > MEASURE_RANGE_R_DETECT) ) {
        // If both range are within wall detecting distance,
        // Use range sensor to get rotational error
        feedback_R = (range_R - range_L - MEASURE_RANGE_R_OFFSET) / 10;
    } else if (_range_running && range_L > MEASURE_RANGE_L_DETECT) {
        // If only left side is within range
        // use the middle value of the right range
        feedback_R = (MEASURE_RANGE_R_MIDDLE - range_L) / 10;
    } else if (_range_running && range_R > MEASURE_RANGE_R_DETECT) {
        // If only right side is within range
        // use the middle value of the left range
        feedback_R = (range_R - MEASURE_RANGE_L_MIDDLE) / 10;
    } else {
        // in open space, use rotary encoder
        feedback_R = speed.left - speed.right;
    }

    int32_t outputR = pid_compute(&g_pid_R, feedback_R);

    if (!_pid_T_running) {
        outputT = 0;
    }
    if (!_pid_R_running) {
        outputR = 0;
    }
    if (!_pid_running) {
        return;
    }

    // Apply to the motor
    motor_speed_percent(CH_LEFT, outputT + outputR);
    motor_speed_percent(CH_RIGHT, outputT - outputR);
    motor_start(CH_BOTH);

    /* If there is change in motion semaphore trigger context switching */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


/*******************************************************************************
 * system functions from @system_control.h
 ******************************************************************************/
// Motor driving
void system_start_driving(void)
{
    _pid_running = 1;
    _pid_T_running = 1;
    _pid_R_running = 1;
}

void system_stop_driving(void)
{
    _pid_running = 0;
    _pid_T_running = 0;
    _pid_R_running = 0;
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);
}

// Range finder control
void system_enable_range_finder(void)
{
    _range_running = 1;
}

void system_disable_range_finder(void)
{
    _range_running = 0;
}

void system_reset_encoder(void)
{
    /* before reset hardware, reset old steps */
    // first compansate values before resetting
    encoder_get(&step, ENCODER_CH_BOTH); 
    step_old.left = MEASURE_ENCODER_DEFAULT - (step.left - step_old.left);
    step_old.right = MEASURE_ENCODER_DEFAULT - (step.right - step_old.right);
    encoder_reset(ENCODER_CH_BOTH);
}

void system_motion_forward_L(void)
{
    _motion_status_L = forward;
}

void system_motion_backward_L(void)
{
    _motion_status_L = backward;
}

void system_motion_forward_R(void)
{
    _motion_status_R = forward;
}

void system_motion_backward_R(void)
{
    _motion_status_R = backward;
}
