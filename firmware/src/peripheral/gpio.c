/*
 * gpio.c
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#include "common_source.h"
#include "gpio.h"

// base name change. Used with msg(). See @base.h
#ifdef KB_MSG_BASE
    #undef KB_MSG_BASE
    #define KB_MSG_BASE "GPIO"
#endif

static int register_callback_(gpio_pin_t pin, void (*callback)(void));
static int set_isr_(gpio_pin_t pin, uint8_t enable);
static void (*callback_list_[])(void);

void gpio_init(gpio_port_t port, gpio_pin_t pin, gpio_init_t *gpio_init)
{
    gpio_enable_clk(port);
    gpio_init->Pin = pin;
    HAL_GPIO_Init(port, gpio_init);
    return;
}

gpio_state_t gpio_read(gpio_port_t port, gpio_pin_t pin)
{
    return HAL_GPIO_ReadPin(port, pin);
}

void gpio_set(gpio_port_t port, gpio_pin_t pin, gpio_state_t state)
{
    HAL_GPIO_WritePin(port, pin, state);
    return;
}


void gpio_toggle(gpio_port_t port, gpio_pin_t pin)
{
    HAL_GPIO_TogglePin(port, pin);
    return;
}


void gpio_enable_clk(gpio_port_t port)
{
    if(port == GPIOA)
    {
        __GPIOA_CLK_ENABLE();
    }
    else if(port == GPIOB)
    {
        __GPIOB_CLK_ENABLE();
    }
    else if(port == GPIOC)
    {
        __GPIOC_CLK_ENABLE();
    }
    else if(port == GPIOD)
    {
        __GPIOD_CLK_ENABLE();
    }
    else if(port == GPIOE)
    {
        __GPIOE_CLK_ENABLE();
    }
    else if(port == GPIOF)
    {
        __GPIOF_CLK_ENABLE();
    }
    else if(port == GPIOG)
    {
        __GPIOG_CLK_ENABLE();
    }
    else if(port == GPIOH)
    {
        __GPIOH_CLK_ENABLE();
    }
    else
    {
        KB_DEBUG_ERROR("Wrong port selected for clock!\r\n");
    }
    return;
}


int gpio_isr_register(gpio_port_t port, gpio_pin_t pin, void (*callback)(void))
{
    return register_callback_(pin, callback);
}


int gpio_isr_deregister(gpio_port_t port, gpio_pin_t pin)
{
    return register_callback_(pin, NULL);
}


int gpio_isr_enable(gpio_port_t port, gpio_pin_t pin, gpio_init_t *gpio_setting, gpio_edge_t edge)
{
    // Set GPIO
    gpio_setting->Mode = ((edge == RISING_EDGE)?GPIO_MODE_IT_RISING:
            (edge == FALLING_EDGE)?GPIO_MODE_IT_FALLING:
            (edge == BOTH_EDGE)?GPIO_MODE_IT_RISING_FALLING:0);
    gpio_init(port, pin, gpio_setting);

    return set_isr_(pin, 1);
}


int gpio_isr_disable(gpio_port_t port, gpio_pin_t pin)
{
    // Set GPIO
    gpio_init_t gpio_setting = {
        .Pin = pin,
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_LOW
    };
    gpio_init(port, pin, &gpio_setting);

    return set_isr_(pin, 0);
}

/******************************************************************************
 * Privates
 ******************************************************************************/

static int register_callback_(gpio_pin_t pin, void (*callback)(void))
{
    // Set NVIC and put the callback
    int idx;
    switch(pin)
    {
    case PIN_0:
        idx = 0;
        break;
    case PIN_1:
        idx = 1;
        break;
    case PIN_2:
        idx = 2;
        break;
    case PIN_3:
        idx = 3;
        break;
    case PIN_4:
        idx = 4;
        break;
    case PIN_5:
        idx = 5;
        break;
    case PIN_6:
        idx = 6;
        break;
    case PIN_7:
        idx = 7;
        break;
    case PIN_8:
        idx = 8;
        break;
    case PIN_9:
        idx = 9;
        break;
    case PIN_10:
        idx = 10;
        break;
    case PIN_11:
        idx = 11;
        break;
    case PIN_12:
        idx = 12;
        break;
    case PIN_13:
        idx = 13;
        break;
    case PIN_14:
        idx = 14;
        break;
    case PIN_15:
        idx = 15;
        break;
    default:
        KB_DEBUG_ERROR("Wrong Pin selected!\r\n");
        return KB_ERROR;
    }
    if(NULL != callback_list_[idx])
    {
        KB_DEBUG_WARNING("Replacing an interrupt handler...\r\n");
    }
    callback_list_[idx] = callback;
    return KB_OK;
}

static int set_isr_(gpio_pin_t pin, uint8_t enable)
{
    // Set NVIC and put the callback
    IRQn_Type irq_num;
    switch(pin)
    {
    case PIN_0:
        irq_num = EXTI0_IRQn;
        break;
    case PIN_1:
        irq_num = EXTI1_IRQn;
        break;
    case PIN_2:
        irq_num = EXTI2_IRQn;
        break;
    case PIN_3:
        irq_num = EXTI3_IRQn;
        break;
    case PIN_4:
        irq_num = EXTI4_IRQn;
        break;
    case PIN_5:
        irq_num = EXTI9_5_IRQn;
        break;
    case PIN_6:
        irq_num = EXTI9_5_IRQn;
        break;
    case PIN_7:
        irq_num = EXTI9_5_IRQn;
        break;
    case PIN_8:
        irq_num = EXTI9_5_IRQn;
        break;
    case PIN_9:
        irq_num = EXTI9_5_IRQn;
        break;
    case PIN_10:
        irq_num = EXTI15_10_IRQn;
        break;
    case PIN_11:
        irq_num = EXTI15_10_IRQn;
        break;
    case PIN_12:
        irq_num = EXTI15_10_IRQn;
        break;
    case PIN_13:
        irq_num = EXTI15_10_IRQn;
        break;
    case PIN_14:
        irq_num = EXTI15_10_IRQn;
        break;
    case PIN_15:
        irq_num = EXTI15_10_IRQn;
        break;
    default:
        KB_DEBUG_ERROR("Wrong Pin selected!\r\n");
        return KB_ERROR;
    }
    HAL_NVIC_SetPriority(irq_num, 15, 0);  // TODO: Find a better group number

    // enable interrupt
    if (enable)
    {
        HAL_NVIC_EnableIRQ(irq_num);
    }
    else
    {
        HAL_NVIC_DisableIRQ(irq_num);
    }
    return KB_OK;
}

static void (*callback_list_[])(void) = {
    NULL,   // EXTI0_IRQn
    NULL,   // EXTI1_IRQn
    NULL,   // EXTI2_IRQn
    NULL,   // EXTI3_IRQn
    NULL,   // EXTI4_IRQn
    NULL,   // 5 // EXTI9_5_IRQn
    NULL,   // 6 // EXTI9_5_IRQn
    NULL,   // 7 // EXTI9_5_IRQn
    NULL,   // 8 // EXTI9_5_IRQn
    NULL,   // 9 // EXTI9_5_IRQn
    NULL,   // 10 // EXTI15_10_IRQn
    NULL,   // 11 // EXTI15_10_IRQn
    NULL,   // 12 // EXTI15_10_IRQn
    NULL,   // 13 // EXTI15_10_IRQn
    NULL,   // 14 // EXTI15_10_IRQn
    NULL   // 15 // EXTI15_10_IRQn
};

/******************************************************************************
 * Interrupt Handlers
 ******************************************************************************/

void EXTI0_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
        if(NULL != callback_list_[0])
        {
            callback_list_[0]();
        }
    }
}
void EXTI1_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_1) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
        if(NULL != callback_list_[1])
        {
            callback_list_[1]();
        }
    }
}
void EXTI2_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
        if(NULL != callback_list_[2])
        {
            callback_list_[2]();
        }
    }
}
void EXTI3_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_3) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
        if(NULL != callback_list_[3])
        {
            callback_list_[3]();
        }
    }
}
void EXTI4_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
        if(NULL != callback_list_[4])
        {
            callback_list_[4]();
        }
    }
}
void EXTI9_5_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
        if(NULL != callback_list_[5])
        {
            callback_list_[5]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
        if(NULL != callback_list_[6])
        {
            callback_list_[6]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_7) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
        if(NULL != callback_list_[7])
        {
            callback_list_[7]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
        if(NULL != callback_list_[8])
        {
            callback_list_[8]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
        if(NULL != callback_list_[9])
        {
            callback_list_[9]();
        }
    }
}
void EXTI15_10_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_10) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
        if(NULL != callback_list_[10])
        {
            callback_list_[10]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_11) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_11);
        if(NULL != callback_list_[11])
        {
            callback_list_[11]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
        if(NULL != callback_list_[12])
        {
            callback_list_[12]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
        if(NULL != callback_list_[13])
        {
            callback_list_[13]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
        if(NULL != callback_list_[14])
        {
            callback_list_[14]();
        }
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
        if(NULL != callback_list_[15])
        {
            callback_list_[15]();
        }
    }
}
