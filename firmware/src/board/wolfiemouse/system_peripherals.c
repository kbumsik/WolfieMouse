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
#include "range.h"
#include "motor.h"
#include "pid.h"
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

// Encoder value
volatile struct encoder_data step, step_old;
volatile struct _speed_struct {
    int32_t left;
    int32_t right;
    int32_t diff;    // left - right
} speed;

// Range finder value
volatile struct range_data range;

// PID handler
pid_handler_t g_pid_T;    // Translational
pid_handler_t g_pid_R; // Rotational

/*******************************************************************************
 * States 
 ******************************************************************************/
enum wheel_state {
    WHEEL_DISABLED, WHEEL_FORWARD, WHEEL_BACKWARD
};

struct _state {
    int hardware;
    int pid;
    int pid_tran;
    int pid_rot;
    int range;
    // enum wheel_state wheel_right;
    // enum wheel_state wheel_left;
};

static volatile struct _state state = {
    .hardware = 0,
    .pid = 0,
    .pid_tran = 0,
    .pid_rot = 0,
    .range = 0,
    // .wheel_right = WHEEL_DISABLED,
    // .wheel_left = WHEEL_DISABLED,
};

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
    range_init();
    
    /* Init encoder */
    encoder_init();
    encoder_get(&step, ENCODER_CH_BOTH);

    /* Set PID controller */
    pid_value_t pid_T_value = {
            .kp = 20,
            .ki = 0.01,
            .kd = 30
    };

    pid_value_t pid_R_value = {
                .kp = 1,
                .ki = 0,
                .kd = 2
    };

    pid_set_pid(&g_pid_T, &pid_T_value);
    pid_reset(&g_pid_T);

    pid_set_pid(&g_pid_R, &pid_R_value);
    pid_reset(&g_pid_R);

    pid_input_setpoint(&g_pid_T, 0);
    pid_input_setpoint(&g_pid_R, 0);

    /* Now peripherals has been initialized */
    state.hardware = 1;
}

/**
 * This overrides original SysTick_hook() located in hooks.c
 * This function is automatically called every 1ms, triggered by SysTick.
 */
#include "config_measurements.h"

void SysTick_hook(void)
{
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (!state.hardware) {
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
    if(state.range) {
        range_get(&range, RANGE_CH_ALL);
    }

    // get errorR
    int32_t feedback_R;
    if(state.range && (range.left > MEASURE_RANGE_L_DETECT)
                        && (range.right > MEASURE_RANGE_R_DETECT) ) {
        // If both range are within wall detecting distance,
        // Use range sensor to get rotational error
        feedback_R = (range.right - range.left - MEASURE_RANGE_R_OFFSET) / 10;
    } else if (state.range && range.left > MEASURE_RANGE_L_DETECT) {
        // If only left side is within range
        // use the middle value of the right range
        feedback_R = (MEASURE_RANGE_R_MIDDLE - range.left) / 10;
    } else if (state.range && range.right > MEASURE_RANGE_R_DETECT) {
        // If only right side is within range
        // use the middle value of the left range
        feedback_R = (range.right - MEASURE_RANGE_L_MIDDLE) / 10;
    } else {
        // in open space, use rotary encoder
        feedback_R = speed.diff * 10;
    }

    int32_t outputR = pid_compute(&g_pid_R, feedback_R);

    if (!state.pid_tran) {
        outputT = 0;
    }
    if (!state.pid_rot) {
        outputR = 0;
    }
    if (!state.pid) {
        return;
    }

    // Apply to the motor
    motor_speed_permyriad(CH_LEFT, outputT + outputR);
    motor_speed_permyriad(CH_RIGHT, outputT - outputR);
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
    state.pid = 1;
    state.pid_tran = 1;
    state.pid_rot = 1;
}

void system_stop_driving(void)
{
    state.pid = 0;
    state.pid_tran = 0;
    state.pid_rot = 0;
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);
}

// Range finder control
void system_enable_range_finder(void)
{
    state.range = 1;
}

void system_disable_range_finder(void)
{
    state.range = 0;
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
