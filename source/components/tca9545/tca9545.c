/*
 * tca9545.c
 *
 *  Created on: Apr 6, 2016
 *      Author: Bumsik Kim
 */
#include "tca9545.h"
#include "timer.h"

#define TCA9545_ADDR	0x70<<1
#define TCA9545_retry	10
#define TCA9545_TIMEOUT	100

#define tca9545RESET_PIN	GPIO_PIN_13
#define tca9545RESET_PORT	GPIOC

#define tca9545_reset()	\
	do { \
    	HAL_GPIO_WritePin(tca9545RESET_PORT, tca9545RESET_PIN, GPIO_PIN_RESET); \
    	HAL_Delay(1); \
    	HAL_GPIO_WritePin(tca9545RESET_PORT, tca9545RESET_PIN, GPIO_PIN_SET); \
    }while(0)

int tca9545_init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	uint8_t buf;

	GPIO_InitStruct.Pin = tca9545RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(tca9545RESET_PORT, &GPIO_InitStruct);
    tca9545_reset();

	/* Init I2C Bus */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 400000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	HAL_I2C_Init(&hi2c1);

	/* Check the chip information */
	buf = tca9545_get();
	return 0;
}
int tca9545_set(tca9545Channel_e ch){
	uint8_t send;
	uint8_t retry = 0;
	switch(ch)
	{
	case Channel_0:
		send = 0x01;
		break;
	case Channel_1:
		send = 0x02;
		break;
	case Channel_2:
		send = 0x04;
		break;
	case Channel_3:
		send = 0x08;
		break;
	default:
		return -1;
	}
	while (HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, TCA9545_ADDR, &send, 1, TCA9545_TIMEOUT))
	{
		DEBUG_ERROR("TCA9545 not found! retry\n");
		if (0 == tca9545_init())
		{
			continue;
		}
		return -1;
	}
	/* FIXME: Delay needed, but what is the minimum? */
	vTimerDelayMicros(100);
	return 0;
}
uint8_t tca9545_get(){
	uint8_t buf = 0;
	uint8_t retry = 0;
	while (HAL_OK != HAL_I2C_Master_Receive(&hi2c1, TCA9545_ADDR, &buf, 1, TCA9545_TIMEOUT))
	{
		retry++;
		if (retry > TCA9545_retry)
		{
			DEBUG_ERROR("Failed to connect TCA9545!\n");
			return 0;
		}
		DEBUG_ERROR("TCA9545 not found! retry\n");
		tca9545_reset();
	}
	return	buf;
}
