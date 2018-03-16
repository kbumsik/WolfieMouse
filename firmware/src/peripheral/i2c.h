/*
 * i2c.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_I2C_H_
#define PERIPHERAL_KB_I2C_H_

#include "common_header.h"
#include "gpio.h"


#if defined(STM32)
    typedef void* i2c_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif

typedef struct{
    uint32_t	frequency;
}i2c_init_t;

#ifdef __cplusplus
extern "C"{
#endif

int i2c_init(i2c_t i2c, i2c_init_t *settings);
// TODO: set address function?
int i2c_sda_pin(i2c_t i2c, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull);
int i2c_scl_pin(i2c_t i2c, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull);

int i2c_send(i2c_t i2c, uint16_t address_target, uint8_t* buf, uint16_t size);
int i2c_send_timeout(i2c_t i2c, uint16_t address_target, uint8_t *buf, uint16_t size, uint32_t timeout);

int i2c_receive(i2c_t i2c, uint16_t address_target, uint8_t* buf, uint16_t size);
int i2c_receive_timeout(i2c_t i2c, uint16_t address_target, uint8_t* buf, uint16_t size, uint32_t timeout);
/*
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout);
*/

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_I2C_H_ */
