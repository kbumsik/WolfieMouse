/*
 * peripherals.c
 *
 *  Created on: Mar 19, 2017
 *      Author: kbumsik
 */

#include "system_config.h"
#include "kb_common_source.h"

#include "kb_gpio.h"
#include "kb_terminal.h"
#include "kb_TCA9545A_i2c_mux.h"
#include "kb_VL6180X_range_finder.h"
#include "kb_HCMS-290X_display.h"
#include "encoder.h"
#include "motor.h"

/*******************************************************************************
 * Global variables
 ******************************************************************************/
/**
 * Range finder value
 * Range_front is the average of range_front_left and range_front_right.
 */
volatile uint8_t range_left = 255, range_right = 255, range_front = 255,
            range_front_left = 255, range_front_right = 255;

// Encoder value
volatile uint32_t left_cnt, right_cnt, left_cnt_old, right_cnt_old;
volatile float left_speed, right_speed, diff_speed;

/*******************************************************************************
 * Static variables
 ******************************************************************************/
static int is_peripherals_initialized = 0;

/*******************************************************************************
 * Function Definition
 ******************************************************************************/
void peripheral_init(void)
{
    kb_gpio_init_t GPIO_InitStruct;

    /* Configure UART */
    kb_terminal_init();

    /* Configure Pushbuttons */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = NOPULL;
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

    /* Configure LED Pins Output Level */
    kb_gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    kb_gpio_set(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    kb_gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_RESET);
    kb_gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_RESET);

    /* Init peripherals for the LED display */
    hcms_290x_init();

    /* Init i2c MUX */
    //tca9545a_init();
    //tca9545a_select_ch(TCA9545A_CH_0);

    /* Init Digital range sensors */
    //vl6180x_init();
    //vl6180x_range_mm();

    /* Init encoder */
    encoder_init();
    left_cnt = encoder_left_count();
    right_cnt = encoder_right_count();

    /* Init motor */
    motor_init();
    motor_stop(CH_BOTH);

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
    left_cnt_old = left_cnt;
    right_cnt_old = right_cnt;
    left_cnt = encoder_left_count();
    right_cnt = encoder_right_count();
    left_speed = (float)(left_cnt - left_cnt_old);// * CONFIG_LEN_PER_CNT;
    right_speed = (float)(right_cnt - right_cnt_old);// * CONFIG_LEN_PER_CNT;
    diff_speed = left_speed - right_speed;
}
