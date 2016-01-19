/*
 * drone_config.h
 *
 *  Created on: Jul 24, 2015
 *      Author: BumSik
 */

#ifndef DRONE_CONFIG_H_
#define DRONE_CONFIG_H_ 100

#include "config.h"

#ifdef __cplusplus
extern "C"{
#endif

extern I2C_HandleTypeDef hi2c3;

void board_Init(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* DRONE_CONFIG_H_ */
