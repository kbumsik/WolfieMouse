/*
 * kb_timer.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_ENCODER_H_
#define PERIPHERAL_KB_ENCODER_H_

/* Includes */
#include "kb_common_header.h"
#include "kb_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Device Specific Enumerations
 ******************************************************************************/
#if defined(STM32)
    typedef enum _kb_tim_device_t {
        KB_ENCODER_TIM1 = 0, KB_ENCODER_TIM2, KB_ENCODER_TIM3, KB_ENCODER_TIM4,
        KB_ENCODER_TIM5, KB_ENCODER_TIM6, KB_ENCODER_TIM7, KB_ENCODER_TIM8, 
        KB_ENCODER_TIM9, KB_ENCODER_TIM10, KB_ENCODER_TIM11, KB_ENCODER_TIM12, 
        KB_ENCODER_TIM13, KB_ENCODER_TIM14
    } kb_encoder_device_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif

/*******************************************************************************
 * Common Enumerations
 ******************************************************************************/
typedef struct _kb_adc_t {
    kb_extlib_handle_t ext_handler;
} kb_encoder_t;

typedef enum {
    CW,
    CCW
} kb_encoder_dir_t;

/*******************************************************************************
 * API object structures
 ******************************************************************************/
typedef struct {
    kb_encoder_device_t device;
    kb_encoder_dir_t direction;
    uint16_t	prescaler;
} kb_encoder_init_t;

/*******************************************************************************
 * API functions
 ******************************************************************************/
int 	kb_encoder_init(kb_encoder_t *obj, kb_encoder_init_t *setting);
int 	kb_encoder_start(kb_encoder_t *obj);
int 	kb_encoder_stop(kb_encoder_t *obj);
int32_t kb_encoder_set(kb_encoder_t *obj, int32_t input);
int32_t kb_encoder_count(kb_encoder_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_ENCODER_H_ */
