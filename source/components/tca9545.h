/*
 * tca9545.h
 *
 *  Created on: Apr 6, 2016
 *      Author: Bumsik Kim
 */

#ifndef TCA9545_H_
#define TCA9545_H_

#include "config.h"
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C"{
#endif

I2C_HandleTypeDef hi2c1; /* In main.cpp */

typedef enum {
	Channel_0 = 0,
	Channel_1,
	Channel_2,
	Channel_3
}tca9545Channel_e;

int tca9545_init(void);
int tca9545_set(tca9545Channel_e ch);
uint8_t tca9545_get();

#ifdef __cplusplus
}
#endif

#endif /* TCA9545_H_ */
