/*
 * tim.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_TIMER_H_
#define PERIPHERAL_KB_TIMER_H_

/* Includes */
#include "common_header.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Device Specific Enumerations
 ******************************************************************************/
#if defined(STM32)
    typedef enum _tim_device_t {
        KB_TIMER_TIM1 = 0, KB_TIMER_TIM2, KB_TIMER_TIM3, KB_TIMER_TIM4, 
        KB_TIMER_TIM5, KB_TIMER_TIM6, KB_TIMER_TIM7, KB_TIMER_TIM8,
        KB_TIMER_TIM9, KB_TIMER_TIM10, KB_TIMER_TIM11, KB_TIMER_TIM12,
        KB_TIMER_TIM13, KB_TIMER_TIM14
    } tim_device_t;

    typedef enum _tim_ch_t{
        CH_1 = TIM_CHANNEL_1, // 0x01U,
        CH_2 = TIM_CHANNEL_2, // 0x02U,
        CH_3 = TIM_CHANNEL_3, // 0x03U,
        CH_4 = TIM_CHANNEL_4  //0x04U,
    } tim_ch_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif


/*******************************************************************************
 * Common Enumerations
 ******************************************************************************/
typedef struct _tim_t {
    extlib_handle_t ext_handler;
} tim_t;

/*******************************************************************************
 * API object structures
 ******************************************************************************/
typedef struct {
    tim_device_t device;
    uint32_t	clock_frequency;
    uint16_t	period;
} pwm_init_t;


/*******************************************************************************
 * API functions
 ******************************************************************************/
// TODO: generic timer function
int tim_ch_pin(tim_device_t dev, tim_ch_t ch, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull);

// PWM functions
int pwm_init(tim_t *obj, pwm_init_t *setting);
int pwm_percent(tim_t *obj, tim_ch_t ch, uint8_t duty_cycle_percent);
// 1 permyriad = 0.01%. 10,000 permyriad = 1%. max 10,000 permyriad
int pwm_permyriad(tim_t *obj, tim_ch_t ch, uint16_t duty_cycle_permyriad);
int pwm_start(tim_t *obj, tim_ch_t ch);
int pwm_stop(tim_t *obj, tim_ch_t ch);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_TIMER_H_ */
