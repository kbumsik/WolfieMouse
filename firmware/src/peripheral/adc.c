/*
 * spi.c
 *
 *  Created on: Oct 25, 2016
 *      Author: Bumsik Kim
 */

#include "common_source.h"
#include "adc.h"

/* Changing base name. Used with msg(). See @base.h */
#ifdef KB_MSG_BASE
    #undef KB_MSG_BASE
    #define KB_MSG_BASE "ADC"
#endif

/* Fuctions that converts KB library name to manufacturer name */
static ADC_TypeDef *_get_device(adc_device_t dev);
static uint32_t _get_channel(adc_ch_t ch);
/* Peripheral clock function */
static void _enable_adc_clk(adc_device_t dev);

/******************************************************************************
 * Public API Functions
 ******************************************************************************/
/* Initialization functions */
int adc_init(adc_t *obj, adc_init_t *settings)
{
    ADC_ChannelConfTypeDef sConfig;
    
    /* Configure the ADC peripheral */
    obj->handler.Instance = _get_device(settings->device);

    /* De-initialize before initialization */
    if (HAL_ADC_DeInit(&obj->handler) != HAL_OK) {
       /* ADC de-initialization Error */
        KB_DEBUG_ERROR("device de-initialization error!\r\n");
        return KB_ERROR;
    }
  
    /* Initialize ADC hardware */
    // Enable ADC Clock first
    _enable_adc_clk(settings->device);
    
    // Asynchronous clock mode, input ADC clock not divided 
    obj->handler.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // 12-bit resolution for converted data 
    obj->handler.Init.Resolution            = ADC_RESOLUTION_12B;
    // Right-alignment for converted data 
    obj->handler.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    // Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) 
    obj->handler.Init.ScanConvMode          = DISABLE;
    // EOC flag picked-up to indicate conversion end 
    obj->handler.Init.EOCSelection          = DISABLE;
    // Continuous mode disabled to have only 1 conversion at each conversion trig 
    obj->handler.Init.ContinuousConvMode    = DISABLE;
    // Parameter discarded because sequencer is disabled 
    obj->handler.Init.NbrOfConversion       = 1;
    // Parameter discarded because sequencer is disabled 
    obj->handler.Init.DiscontinuousConvMode = DISABLE;
    // Parameter discarded because sequencer is disabled 
    obj->handler.Init.NbrOfDiscConversion   = 0;
    // Software start to trig the 1st conversion manually, without external event 
    obj->handler.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    // Parameter discarded because software trigger chosen 
    obj->handler.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    // DMA one-shot mode selected (not applied to this example)
    obj->handler.Init.DMAContinuousRequests = DISABLE;

    if (HAL_ADC_Init(&obj->handler) != HAL_OK) {
        /* ADC initialization Error */
        KB_DEBUG_ERROR("device initialization error!\r\n");
        return KB_ERROR;
    }

    /* Configure ADC regular channel */
    // Sampled channel number
    sConfig.Channel      = _get_channel(settings->channel);
    // Rank of sampled channel
    sConfig.Rank         = 1;
    // Sampling time (number of clock cycles unit)
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    // Parameter discarded because offset correction is disabled
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(&obj->handler, &sConfig) != HAL_OK) {
        /* Channel Configuration Error */
        KB_DEBUG_ERROR("Channel configuration Error!\r\n");
        return KB_ERROR;
    }

    /* Creating mutex */
#if KB_USE_PERIPHERAL_FREERTOS_LOCK
    obj->lock = xSemaphoreCreateMutex();
    if(obj->lock == NULL) {
        /* Mutex creating Error */
        KB_DEBUG_ERROR("device mutex creation error!\r\n");
        return KB_ERROR;
    }
#endif
    return KB_OK;
}

/* Configuration functions */
int adc_pin(gpio_port_t port, gpio_pin_t pin)
{
    /* Enable GPIO clock */
    gpio_enable_clk(port);

    /* ADC Channel GPIO pin configuration */
    gpio_init_t gpio_setting = {
        .Mode = GPIO_MODE_ANALOG,
        .Pull = NOPULL,
        .Speed = GPIO_SPEED_FREQ_VERY_HIGH // 50MHz
    };
    gpio_init(port, pin, &gpio_setting);
    return KB_OK;
}

/* Operation functions */
uint32_t adc_measure(adc_t *obj)
{   
    /*##-3- Start the conversion process #######################################*/
    if (HAL_ADC_Start(&obj->handler) != HAL_OK) {
      /* Start Conversation Error */
      Error_Handler();
    }

    /*##-4- Wait for the end of conversion #####################################*/
    /*  Before starting a new conversion, you need to check the current state of
         the peripheral; if it�s busy you need to wait for the end of current
         conversion before starting a new one.
         For simplicity reasons, this example is just waiting till the end of the
         conversion, but application may perform other tasks while conversion
         operation is ongoing. */
    if (HAL_ADC_PollForConversion(&obj->handler, 10) != HAL_OK) {
      /* End Of Conversion flag not set on time */
      Error_Handler();
    }

    /* Check if the continuous conversion of regular channel is finished */
    if ((HAL_ADC_GetState(&obj->handler) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG) {
        /*##-5- Get the converted value of regular channel  ########################*/
        return HAL_ADC_GetValue(&obj->handler);
    }
    KB_DEBUG_ERROR("Conversion error!\r\n");
    return KB_ERROR;
}

/******************************************************************************
 * Private Functions
 ******************************************************************************/
static ADC_TypeDef *_get_device(adc_device_t dev)
{
    static ADC_TypeDef *const _device_table[] = {
        ADC1, ADC2, ADC3
    };
    return _device_table[(int)dev];
}


static uint32_t _get_channel(adc_ch_t ch)
{
    static const uint32_t _channel_table[] = {
        ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3, ADC_CHANNEL_4, 
        ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_8, ADC_CHANNEL_9, 
        ADC_CHANNEL_10, ADC_CHANNEL_11, ADC_CHANNEL_12, ADC_CHANNEL_13, 
        ADC_CHANNEL_14, ADC_CHANNEL_15
    };
    return _channel_table[(int)ch];
}


static void _enable_adc_clk (adc_device_t dev)
{
    switch (dev) {
    case KB_ADC1:
        __ADC1_CLK_ENABLE();
        break;
    case KB_ADC2:
        __ADC2_CLK_ENABLE();
        break;
    case KB_ADC3:
        __ADC3_CLK_ENABLE();
        break;
    default:
        KB_DEBUG_ERROR("Wrong ADC device selected!\r\n");
    }
    return;
}
