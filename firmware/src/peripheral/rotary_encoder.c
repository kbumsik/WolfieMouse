/*
 * timer.c
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#include "common_source.h"
#include "rotary_encoder.h"
#include "alternate_pins.h"

// C standard library
#include <stdlib.h> // for malloc()

// base name change. Used with msg(). See @base.h
#ifdef KB_MSG_BASE
    #undef KB_MSG_BASE
    #define KB_MSG_BASE "ENCODER"
#endif

/* Fuctions that converts KB's' device name to manufacturer name */
static TIM_TypeDef *_get_device(rotary_encoder_device_t dev);
/* Get a HAL library handler from the object */
static inline TIM_HandleTypeDef * _get_handler (rotary_encoder_t *obj);
/* Peripheral clock function */
static void _enable_device_clk (rotary_encoder_device_t dev);

/******************************************************************************
 * Public API Functions
 ******************************************************************************/
int rotary_encoder_init(rotary_encoder_t *obj, rotary_encoder_init_t *setting)
{
    /* allocate device handler for HAL driver */
    obj->ext_handler = (extlib_handle_t)malloc(sizeof(TIM_HandleTypeDef));
    TIM_HandleTypeDef* handler = _get_handler(obj);
    if (NULL == handler) {
        KB_DEBUG_ERROR("Memory allocation error!\r\n");
        return KB_ERROR;
    }

    /* Associate the TIM device */
    handler->Instance = _get_device(setting->device);
    
    /* De-initialize before initialization */
    if (HAL_TIM_Encoder_DeInit(handler) != HAL_OK) {
       /* ADC de-initialization Error */
        KB_DEBUG_ERROR("device de-initialization error!\r\n");
        return KB_ERROR;
    }
    
    /* Initialize TIM hardware */
    _enable_device_clk(setting->device); // Enable device Clock first

    // TIM device setting
    TIM_Encoder_InitTypeDef rotary_encoder_config;
    handler->Init.Period = 0xffffffff; //0xFFFF
    handler->Init.Prescaler = setting->prescaler;
    handler->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    handler->Init.RepetitionCounter = 0;

    rotary_encoder_config.EncoderMode = TIM_ENCODERMODE_TI12;

    switch(setting->direction)
    {
    case CW:
        handler->Init.CounterMode = TIM_COUNTERMODE_UP;

        rotary_encoder_config.IC1Polarity = TIM_ICPOLARITY_RISING; //TIM_ICPOLARITY_RISING;
        rotary_encoder_config.IC1Selection = TIM_ICSELECTION_DIRECTTI;
        rotary_encoder_config.IC1Prescaler = TIM_ICPSC_DIV1;
        rotary_encoder_config.IC1Filter = 0xf;

        rotary_encoder_config.IC2Polarity = TIM_ICPOLARITY_RISING; //TIM_ICPOLARITY_RISING;
        rotary_encoder_config.IC2Selection = TIM_ICSELECTION_DIRECTTI;
        rotary_encoder_config.IC2Prescaler = TIM_ICPSC_DIV1;
        rotary_encoder_config.IC2Filter = 0xf;
        break;
    case CCW:
        handler->Init.CounterMode = TIM_COUNTERMODE_UP;

        rotary_encoder_config.IC1Polarity = TIM_ICPOLARITY_FALLING; //TIM_ICPOLARITY_RISING;
        rotary_encoder_config.IC1Selection = TIM_ICSELECTION_DIRECTTI;
        rotary_encoder_config.IC1Prescaler = TIM_ICPSC_DIV1;
        rotary_encoder_config.IC1Filter = 0xf;

        rotary_encoder_config.IC2Polarity = TIM_ICPOLARITY_RISING; //TIM_ICPOLARITY_RISING;
        rotary_encoder_config.IC2Selection = TIM_ICSELECTION_DIRECTTI;
        rotary_encoder_config.IC2Prescaler = TIM_ICPSC_DIV1;
        rotary_encoder_config.IC2Filter = 0xf;
        break;
    default:
        KB_DEBUG_ERROR("Incorrect encoder direction");
        return KB_ERROR;
    }

    int8_t status = HAL_TIM_Encoder_Init(handler, &rotary_encoder_config);
    KB_CONVERT_STATUS(status);
    if (KB_OK != status) {
        KB_DEBUG_WARNING("Error init encoder!\r\n");
    }
    return status;
}


int rotary_encoder_start(rotary_encoder_t *obj)
{
    // get handler and enable timer
    TIM_HandleTypeDef* handler = _get_handler(obj);
    if (NULL == handler) {
        return KB_ERROR;
    }

    int8_t status = HAL_TIM_Encoder_Start(handler, TIM_CHANNEL_ALL);
    KB_CONVERT_STATUS(status);
    if (KB_OK != status) {
        KB_DEBUG_WARNING("Error starting Encoder!\r\n");
    }
    return status;
}


int rotary_encoder_stop(rotary_encoder_t *obj)
{
    // get handler and enable timer
    TIM_HandleTypeDef* handler = _get_handler(obj);
    if (NULL == handler) {
        return KB_ERROR;
    }

    int8_t status = HAL_TIM_Encoder_Stop(handler, TIM_CHANNEL_ALL);
    KB_CONVERT_STATUS(status);
    if (KB_OK != status) {
        KB_DEBUG_WARNING("Error stopping Encoder!\r\n");
    }
    return status;
}


int32_t rotary_encoder_set(rotary_encoder_t *obj, int32_t input)
{
    // get handler and enable timer
    TIM_HandleTypeDef* handler = _get_handler(obj);
    if (NULL == handler) {
        return 0;
    }
    handler->Instance->CNT = input;
    return handler->Instance->CNT;
}


int32_t rotary_encoder_count(rotary_encoder_t *obj)
{
    // get handler and enable timer
    TIM_HandleTypeDef* handler = _get_handler(obj);
    if (NULL == handler) {
        return 0;
    }
    return handler->Instance->CNT;
}

/******************************************************************************
 * Private Functions
 ******************************************************************************/
static TIM_TypeDef *_get_device(rotary_encoder_device_t dev)
{
    static TIM_TypeDef *const _device_table[] = {
        TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM8, TIM9, TIM10, TIM11, 
        TIM12, TIM13, TIM14
    };
    return _device_table[(int)dev];
}


static inline TIM_HandleTypeDef * _get_handler (rotary_encoder_t *obj)
{
    return (TIM_HandleTypeDef*) obj->ext_handler;
}


static void _enable_device_clk (rotary_encoder_device_t dev)
{
    switch (dev) {
    case KB_ENCODER_TIM1:     
        __TIM1_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM2: 
        __TIM2_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM3: 
        __TIM3_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM4: 
        __TIM4_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM5: 
        __TIM5_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM6: 
        __TIM6_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM7: 
        __TIM7_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM8: 
        __TIM8_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM9: 
        __TIM9_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM10: 
        __TIM10_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM11: 
        __TIM11_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM12: 
        __TIM12_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM13: 
        __TIM13_CLK_ENABLE();
        break;
    case KB_ENCODER_TIM14: 
        __TIM14_CLK_ENABLE();
        break;
    default:
        KB_DEBUG_ERROR("Wrong TIM device!\r\n");
    }
    return;
}
