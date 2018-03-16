/*
 * tim.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_ENCODER_H_
#define PERIPHERAL_KB_ENCODER_H_

/* Includes */
#include "common_header.h"
#include "tim.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Device Specific Enumerations
 ******************************************************************************/
#if defined(STM32)
    typedef enum _rotary_encoder_device_t {
        KB_ENCODER_TIM1 = 0, KB_ENCODER_TIM2, KB_ENCODER_TIM3, KB_ENCODER_TIM4,
        KB_ENCODER_TIM5, KB_ENCODER_TIM6, KB_ENCODER_TIM7, KB_ENCODER_TIM8, 
        KB_ENCODER_TIM9, KB_ENCODER_TIM10, KB_ENCODER_TIM11, KB_ENCODER_TIM12, 
        KB_ENCODER_TIM13, KB_ENCODER_TIM14
    } rotary_encoder_device_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif

/*******************************************************************************
 * Common Enumerations
 ******************************************************************************/
typedef struct _adc_t {
    extlib_handle_t ext_handler;
} rotary_encoder_t;

typedef enum {
    CW,
    CCW
} rotary_encoder_dir_t;

/*******************************************************************************
 * API object structures
 ******************************************************************************/
typedef struct {
    rotary_encoder_device_t device;
    rotary_encoder_dir_t direction;
    uint16_t	prescaler;
} rotary_encoder_init_t;

/*******************************************************************************
 * API functions
 ******************************************************************************/
int 	rotary_encoder_init(rotary_encoder_t *obj, rotary_encoder_init_t *setting);
int 	rotary_encoder_start(rotary_encoder_t *obj);
int 	rotary_encoder_stop(rotary_encoder_t *obj);
int32_t rotary_encoder_set(rotary_encoder_t *obj, int32_t input);
int32_t rotary_encoder_count(rotary_encoder_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_ENCODER_H_ */
