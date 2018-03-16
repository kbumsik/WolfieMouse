/*
 * kb_i2c.c
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#include "kb_common_source.h"
#include "kb_i2c.h"
#include "kb_alternate_pins.h"

// base name change. Used with kb_msg(). See @kb_base.h
#ifdef KB_MSG_BASE
    #undef KB_MSG_BASE
    #define KB_MSG_BASE "I2C"
#endif

static I2C_HandleTypeDef *get_handler (kb_i2c_t i2c);
static void enable_i2c_clk_ (kb_i2c_t i2c);

#if defined(STM32F446xx)
    static I2C_HandleTypeDef i2c_1_h_ = {.Instance = I2C1};
    static I2C_HandleTypeDef i2c_2_h_ = {.Instance = I2C2};
    static I2C_HandleTypeDef i2c_3_h_ = {.Instance = I2C3};
#else
    #error "Please define device! " __FILE__ "\n"
#endif


int kb_i2c_init(kb_i2c_t i2c, kb_i2c_init_t *settings)
{
    // select handler
    I2C_HandleTypeDef* handler = get_handler(i2c);
    if (NULL == handler)
    {
        return KB_ERROR;
    }
    enable_i2c_clk_(i2c);

    // Init I2C Bus
    handler->Init.ClockSpeed = settings->frequency;
    handler->Init.DutyCycle = I2C_DUTYCYCLE_2;
    handler->Init.OwnAddress1 = 0;
    handler->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    handler->Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    handler->Init.OwnAddress2 = 0;
    handler->Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    handler->Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;

    int8_t status = HAL_I2C_Init(handler);;
    KB_CONVERT_STATUS(status);
    if (status != KB_OK)
    {
        KB_DEBUG_ERROR("Error initializing.\r\n");
    }
    return  status;
}


int kb_i2c_sda_pin(kb_i2c_t i2c, kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_pull_t pull)
{
    uint32_t alternate = GPIO_I2C_SDA_AF_(i2c, port, pin);
    if (alternate == KB_WRONG_PIN)
    {
        KB_DEBUG_ERROR("Wrong SDA pin! Find a correct one.\r\n");
        return KB_ERROR;
    }
    kb_gpio_enable_clk(port);
    // Init GPIOs
    kb_gpio_init_t gpio_setting = {
        .Mode = GPIO_MODE_AF_OD,
        .Pull = pull,
        .Alternate = alternate,
        .Speed = GPIO_SPEED_HIGH // 50MHz
    };
    kb_gpio_init(port, pin, &gpio_setting);
    return KB_OK;
}


int kb_i2c_scl_pin(kb_i2c_t i2c, kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_pull_t pull)
{
    uint32_t alternate = GPIO_I2C_SCL_AF_(i2c, port, pin);
    if (alternate == KB_WRONG_PIN)
    {
        KB_DEBUG_ERROR("Wrong SCL pin! Find a correct one.\r\n");
        return KB_ERROR;
    }
    kb_gpio_enable_clk(port);
    // Init GPIOs
    kb_gpio_init_t gpio_setting = {
        .Mode = GPIO_MODE_AF_OD,
        .Pull = pull,
        .Alternate = alternate,
        .Speed = GPIO_SPEED_HIGH // 50MHz
    };
    kb_gpio_init(port, pin, &gpio_setting);
    return KB_OK;
}


inline int kb_i2c_send(kb_i2c_t i2c, uint16_t address_target, uint8_t* buf, uint16_t size)
{
    return kb_i2c_send_timeout(i2c, address_target, buf, size, TIMEOUT_MAX);
}


int kb_i2c_send_timeout(kb_i2c_t i2c, uint16_t address_target, uint8_t* buf, uint16_t size, uint32_t timeout)
{
    // select handler
    I2C_HandleTypeDef* handler = get_handler(i2c);
    if (NULL == handler) {
        return KB_ERROR;
    }
    // target address is needed to be shit by 1
    address_target <<= 1;

    int8_t status = HAL_I2C_Master_Transmit(handler, address_target, buf, size, timeout);
    KB_CONVERT_STATUS(status);
    if (status != KB_OK)
    {
        KB_DEBUG_ERROR("Error in sending.\r\n");
    }
    return  status;
}


int kb_i2c_receive(kb_i2c_t i2c, uint16_t address_target, uint8_t* buf, uint16_t size)
{
    return kb_i2c_receive_timeout(i2c, address_target, buf, size, TIMEOUT_MAX);
}


int kb_i2c_receive_timeout(kb_i2c_t i2c, uint16_t address_target, uint8_t* buf, uint16_t size, uint32_t timeout)
{
    // select handler
    I2C_HandleTypeDef* handler = get_handler(i2c);
    if (NULL == handler) {
        return KB_ERROR;
    }
    // target address is needed to be shit by 1
    address_target <<= 1;

    int8_t status = HAL_I2C_Master_Receive(handler, address_target, buf, size, timeout);
    KB_CONVERT_STATUS(status);
    if (status != KB_OK)
    {
        KB_DEBUG_ERROR("Error in sending.\r\n");
    }
    return  status;
}

/******************************************************************************
 * Private Functions
 ******************************************************************************/

static I2C_HandleTypeDef *get_handler (kb_i2c_t i2c)
{
    if (i2c == I2C1)
    {
        return &i2c_1_h_;
    }
    else if (i2c == I2C2)
    {
        return &i2c_2_h_;
    }
    else if (i2c == I2C3)
    {
        return &i2c_3_h_;
    }
    else
    {
        KB_DEBUG_ERROR("Wrong I2C device selected!\r\n");
        return NULL;
    }
}


static void enable_i2c_clk_ (kb_i2c_t i2c)
{
    if (i2c == I2C1)
    {
        __I2C1_CLK_ENABLE();
    }
    else if (i2c == I2C2)
    {
        __I2C2_CLK_ENABLE();
    }
    else if (i2c == I2C3)
    {
        __I2C3_CLK_ENABLE();
    }
    else
    {
        KB_DEBUG_ERROR("Wrong I2C device selected!\r\n");
    }
    return;
}
