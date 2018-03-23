/*
 * timer.c
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#include "common_source.h"
#include "tim.h"
#include "alternate_pins.h"
#include <stdlib.h>

// base name change. Used with msg(). See @base.h
#ifdef KB_MSG_BASE
    #undef KB_MSG_BASE
    #define KB_MSG_BASE "TIMER"
#endif

static TIM_TypeDef *_get_device(tim_device_t dev);
/* Get a HAL library handler from the object */
static inline TIM_HandleTypeDef * _get_handler (tim_t *obj);
static uint32_t _get_bus_freq(tim_device_t timer);
static void _enable_device_clk (tim_device_t timer);


/******************************************************************************
 * Public API Functions
 ******************************************************************************/
int tim_ch_pin(tim_device_t dev, tim_ch_t channel, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull)
{
    uint32_t alternate;

    /* Get device and activate pin */
    TIM_TypeDef* device = _get_device(dev);
    switch (channel) {
    case CH_1:
        alternate = GPIO_TIM_CH1_AF_(device, port, pin);
        break;
    case CH_2:
        alternate = GPIO_TIM_CH2_AF_(device, port, pin);
        break;
    case CH_3:
        alternate = GPIO_TIM_CH3_AF_(device, port, pin);
        break;
    case CH_4:
        alternate = GPIO_TIM_CH4_AF_(device, port, pin);
        break;
    default:
        KB_DEBUG_ERROR("Choose correct channel!\r\n");
        return KB_ERROR;
    }
    if (alternate == KB_WRONG_PIN) {
        KB_DEBUG_ERROR("Wrong MOSI pin! Find a correct one.\r\n");
        return KB_ERROR;
    }
    gpio_enable_clk(port);
    // Init GPIOs
    gpio_init_t gpio_setting = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = pull,
        .Alternate = alternate,
        .Speed = GPIO_SPEED_FREQ_VERY_HIGH // 50MHz
    };
    gpio_init(port, pin, &gpio_setting);
    return KB_OK;
}

// PWM functions
int pwm_init(tim_t *obj, pwm_init_t *setting)
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
    if (HAL_TIM_PWM_DeInit(handler) != HAL_OK) {
       /* ADC de-initialization Error */
        KB_DEBUG_ERROR("device de-initialization error!\r\n");
        return KB_ERROR;
    }

    /* Initialize TIM hardware */
    _enable_device_clk(setting->device); // Enable device Clock first

    // Timer init
    handler->Init.CounterMode = TIM_COUNTERMODE_UP;
    handler->Init.Period = setting->period;
    handler->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    handler->State = HAL_TIM_STATE_RESET;

    //
    /*	f_tim = f_clk/(psc + 1).  (from datasheet)
     * 	psc = f_clk/f_tim - 1
     */
    uint32_t freq;
    handler->Init.Prescaler = _get_bus_freq(setting->device)/setting->clock_frequency - 1;
    freq = _get_bus_freq(setting->device)/(handler->Init.Prescaler + 1);	// TODO: manage it more nicely.
    if (handler->Init.Prescaler > UINT16_MAX) {
        handler->Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
        handler->Init.Prescaler >>= 1;
        freq = (_get_bus_freq(setting->device)>>1)/(handler->Init.Prescaler + 1);
        if (handler->Init.Prescaler > UINT16_MAX) {
            handler->Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
            handler->Init.Prescaler >>= 1;
            freq = (_get_bus_freq(setting->device)>>1)/(handler->Init.Prescaler + 1);
        }
    }
    KB_DEBUG_MSG("Requested clock freq: %lu\r\n", setting->clock_frequency);
    KB_DEBUG_MSG("Requested pulse freq: %lu\r\n", setting->clock_frequency/setting->period);
    KB_DEBUG_MSG("Selected clock freq: %lu\r\n", freq);
    KB_DEBUG_MSG("Selected pulse freq: %lu\r\n", freq/handler->Init.Period);
    // good to go

    int8_t status = HAL_TIM_PWM_Init(handler);
    KB_CONVERT_STATUS(status);
    if (KB_OK != status) {
        KB_DEBUG_WARNING("Error init timer device!\r\n");
    }
    return  (status_t)status;
}


int pwm_percent(tim_t *obj, tim_ch_t channel, uint8_t duty_cycle_percent)
{
    return pwm_permyriad(obj, channel, duty_cycle_percent * 100);
}

// 1 permyriad = 0.01%. 10,000 permyriad = 1%. max 10,000 permyriad
int pwm_permyriad(tim_t *obj, tim_ch_t channel, uint16_t duty_cycle_permyriad)
{
    // select handler
    TIM_HandleTypeDef* handler = _get_handler(obj);
    if (NULL == handler) {
        return KB_ERROR;
    }

    // make configuration
    TIM_OC_InitTypeDef config;
    config.OCMode = TIM_OCMODE_PWM1;
    config.OCPolarity = TIM_OCPOLARITY_HIGH;
    config.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    config.OCFastMode = TIM_OCFAST_DISABLE;
    config.OCIdleState = TIM_OCIDLESTATE_RESET;
    config.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    // setting duty cycle
    if(duty_cycle_permyriad > 10000) {
        KB_DEBUG_WARNING("Duty cycle is > 10,000 permyriad.\r\n");
        duty_cycle_permyriad = 10000;
    }
    uint16_t period = handler->Init.Period;
    uint16_t pulse_width = ((uint32_t)period * duty_cycle_permyriad)/10000;
    config.Pulse = pulse_width;

    int8_t status = HAL_TIM_PWM_ConfigChannel(handler, &config, channel);
    KB_CONVERT_STATUS(status);
    if (KB_OK != status) {
        KB_DEBUG_WARNING("Error setting PWM ch!\r\n");
    }
    return  (status_t)status;
}


int pwm_start(tim_t *obj, tim_ch_t channel)
{
    // get handler and enable timer
    TIM_HandleTypeDef* handler = _get_handler(obj);
    if (NULL == handler) {
        return KB_ERROR;
    }

    int8_t status;
    status = HAL_TIM_PWM_Start(handler, (uint32_t)channel);

    KB_CONVERT_STATUS(status);
    if (KB_OK != status) {
        KB_DEBUG_WARNING("Error starting PWM ch!\r\n");
    }
    return status;
}


int pwm_stop(tim_t *obj, tim_ch_t channel)
{
    // get handler and enable timer
    TIM_HandleTypeDef* handler = _get_handler(obj);
    if (NULL == handler) {
        return KB_ERROR;
    }

    int8_t status;
    switch (channel) {
    case CH_1:
        status = HAL_TIM_PWM_Stop(handler, TIM_CHANNEL_1);
        break;
    case CH_2:
        status = HAL_TIM_PWM_Stop(handler, TIM_CHANNEL_2);
        break;
    case CH_3:
        status = HAL_TIM_PWM_Stop(handler, TIM_CHANNEL_3);
        break;
    case CH_4:
        status = HAL_TIM_PWM_Stop(handler, TIM_CHANNEL_4);
        break;
    default:
        KB_DEBUG_ERROR("Choose correct channel!\r\n");
        return KB_ERROR;
    }
    KB_CONVERT_STATUS(status);
    if (KB_OK != status) {
        KB_DEBUG_WARNING("Error stopping PWM ch!\r\n");
    }
    return status;
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/
static TIM_TypeDef *_get_device(tim_device_t dev)
{
    static TIM_TypeDef *const _device_table[] = {
        TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM8, TIM9, TIM10, TIM11, 
        TIM12, TIM13, TIM14
    };
    return _device_table[(int)dev];
}


static inline TIM_HandleTypeDef * _get_handler (tim_t *obj)
{
    return (TIM_HandleTypeDef*) obj->ext_handler;
}

static uint32_t _get_bus_freq(tim_device_t timer)
{
    RCC_ClkInitTypeDef rcc_config;
    uint32_t flash_latency;
    uint8_t tim_multiplier = 1;
    HAL_RCC_GetClockConfig(&rcc_config, &flash_latency);
    if ((timer == KB_TIMER_TIM2) || (timer == KB_TIMER_TIM3) ||
        (timer == KB_TIMER_TIM4) || (timer == KB_TIMER_TIM5) ||
        (timer == KB_TIMER_TIM6) || (timer == KB_TIMER_TIM7) ||
        (timer == KB_TIMER_TIM12) || (timer == KB_TIMER_TIM13) ||
        (timer == KB_TIMER_TIM14)) {	// APB1
        if (rcc_config.APB1CLKDivider != RCC_HCLK_DIV1) {	// See RCC datasheet
            tim_multiplier = 2;
        }
        return HAL_RCC_GetPCLK1Freq() * tim_multiplier;
    } else if ((timer == KB_TIMER_TIM1) || (timer == KB_TIMER_TIM8) ||
            (timer == KB_TIMER_TIM9) || (timer == KB_TIMER_TIM10) || 
            (timer == KB_TIMER_TIM11)) {	// APB2
        if (rcc_config.APB2CLKDivider != RCC_HCLK_DIV1) {	// See RCC datasheet
            tim_multiplier = 2;
        }
        return HAL_RCC_GetPCLK2Freq() * tim_multiplier;
    }
    KB_DEBUG_ERROR("Wrong TIM device! Find a correct one.\r\n");
    return 0;
}


static void _enable_device_clk (tim_device_t dev)
{
    switch (dev) {
    case KB_TIMER_TIM1:     
        __TIM1_CLK_ENABLE();
        break;
    case KB_TIMER_TIM2: 
        __TIM2_CLK_ENABLE();
        break;
    case KB_TIMER_TIM3: 
        __TIM3_CLK_ENABLE();
        break;
    case KB_TIMER_TIM4: 
        __TIM4_CLK_ENABLE();
        break;
    case KB_TIMER_TIM5: 
        __TIM5_CLK_ENABLE();
        break;
    case KB_TIMER_TIM6: 
        __TIM6_CLK_ENABLE();
        break;
    case KB_TIMER_TIM7: 
        __TIM7_CLK_ENABLE();
        break;
    case KB_TIMER_TIM8: 
        __TIM8_CLK_ENABLE();
        break;
    case KB_TIMER_TIM9: 
        __TIM9_CLK_ENABLE();
        break;
    case KB_TIMER_TIM10: 
        __TIM10_CLK_ENABLE();
        break;
    case KB_TIMER_TIM11: 
        __TIM11_CLK_ENABLE();
        break;
    case KB_TIMER_TIM12: 
        __TIM12_CLK_ENABLE();
        break;
    case KB_TIMER_TIM13: 
        __TIM13_CLK_ENABLE();
        break;
    case KB_TIMER_TIM14: 
        __TIM14_CLK_ENABLE();
        break;
    default:
        KB_DEBUG_ERROR("Wrong TIM device!\r\n");
    }
    return;
}
