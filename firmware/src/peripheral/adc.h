/*
 * spi.h
 *
 *  Created on: Oct 25, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_ADC_H_
#define PERIPHERAL_KB_ADC_H_

#include "common_header.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Forward declaration and matching manufacturer drivers.
 * API users can ignore this section
 ******************************************************************************/
#if defined(STM32)
    /* Match manufacturer library structures */
    typedef ADC_HandleTypeDef   adc_internal_t;
#else
    #error "Please include config.h in the source file or define device driver! (e.g. USE_HAL_DRIVER)."
#endif

/*******************************************************************************
 * Device Specific Enumerations
 ******************************************************************************/
#if defined(STM32)
    typedef enum _adc_device_t {
        KB_ADC1 = 0, KB_ADC2, KB_ADC3
    } adc_device_t;

    typedef enum _adc_ch_t {
        KB_ADC_CH0 = 0, KB_ADC_CH1 , KB_ADC_CH2 , KB_ADC_CH3 , KB_ADC_CH4 , 
        KB_ADC_CH5    , KB_ADC_CH6 , KB_ADC_CH7 , KB_ADC_CH8 , KB_ADC_CH9 ,
        KB_ADC_CH10   , KB_ADC_CH11, KB_ADC_CH12, KB_ADC_CH13, KB_ADC_CH14,
        KB_ADC_CH15
    } adc_ch_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif

/*******************************************************************************
 * API object structures
 ******************************************************************************/
typedef struct _adc_t {
    adc_internal_t handler;
#ifdef KB_USE_PERIPHERAL_FREERTOS_LOCK
    SemaphoreHandle_t lock; // mutex lock
#endif
} adc_t;

typedef struct _adc_init_t {
    adc_device_t device;
    adc_ch_t channel;
} adc_init_t;

/*******************************************************************************
 * API functions
 ******************************************************************************/
/* Initialization functions */
int adc_init(adc_t *obj, adc_init_t *settings);
int adc_pin(gpio_port_t port, gpio_pin_t pin);

/* Configuration functions */

/* Operation functions */
uint32_t adc_measure(adc_t *adc);

/* Lock operation */


#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_ADC_H_ */
