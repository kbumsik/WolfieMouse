/*
 * pushbutton.c
 *
 *  Created on: Sep 22, 2016
 *      Author: Bumsik Kim
 */

#include "pushbutton.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

void pb_init()
{
	GPIO_InitTypeDef pb_setting = {
			.Pin = PB1_PIN,
			.Mode = GPIO_MODE_INPUT,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH
	};
	HAL_GPIO_Init(PB1_PORT, &pb_setting);
	pb_setting.Pin = PB2_PIN;
	HAL_GPIO_Init(PB2_PORT, &pb_setting);
}

int pb_read(pb_t pushbutton)
{
	switch(pushbutton)
	{
	case pb1:
		return HAL_GPIO_ReadPin(PB1_PORT, PB1_PIN);
	case pb2:
		return HAL_GPIO_ReadPin(PB2_PORT, PB2_PIN);
	default:
		return -1;
	}
}
