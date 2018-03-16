/*
 * kb_timer.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_TIMER_H_
#define PERIPHERAL_KB_TIMER_H_

/* Includes */
#include "kb_common_header.h"
#include "kb_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Device Specific Enumerations
 ******************************************************************************/
#if defined(STM32)
    typedef enum _kb_timer_device_t {
        KB_TIMER_TIM1 = 0, KB_TIMER_TIM2, KB_TIMER_TIM3, KB_TIMER_TIM4, 
        KB_TIMER_TIM5, KB_TIMER_TIM6, KB_TIMER_TIM7, KB_TIMER_TIM8,
        KB_TIMER_TIM9, KB_TIMER_TIM10, KB_TIMER_TIM11, KB_TIMER_TIM12,
        KB_TIMER_TIM13, KB_TIMER_TIM14
    } kb_timer_device_t;

    typedef enum _kb_timer_ch_t{
        CH_1 = TIM_CHANNEL_1, // 0x01U,
        CH_2 = TIM_CHANNEL_2, // 0x02U,
        CH_3 = TIM_CHANNEL_3, // 0x03U,
        CH_4 = TIM_CHANNEL_4  //0x04U,
    } kb_timer_ch_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif


/*******************************************************************************
 * Common Enumerations
 ******************************************************************************/
typedef struct _kb_timer_t {
    kb_extlib_handle_t ext_handler;
} kb_timer_t;

/*******************************************************************************
 * API object structures
 ******************************************************************************/
typedef struct {
    kb_timer_device_t device;
    uint32_t	clock_frequency;
    uint16_t	period;
} kb_pwm_init_t;


/*******************************************************************************
 * API functions
 ******************************************************************************/
// TODO: generic timer function
int kb_timer_ch_pin(kb_timer_device_t dev, kb_timer_ch_t ch, kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_pull_t pull);

// PWM functions
int kb_pwm_init(kb_timer_t *obj, kb_pwm_init_t *setting);
int kb_pwm_percent(kb_timer_t *obj, kb_timer_ch_t ch, uint8_t duty_cycle_percent);
// 1 permyriad = 0.01%. 10,000 permyriad = 1%. max 10,000 permyriad
int kb_pwm_permyriad(kb_timer_t *obj, kb_timer_ch_t ch, uint16_t duty_cycle_permyriad);
int kb_pwm_start(kb_timer_t *obj, kb_timer_ch_t ch);
int kb_pwm_stop(kb_timer_t *obj, kb_timer_ch_t ch);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_TIMER_H_ */
