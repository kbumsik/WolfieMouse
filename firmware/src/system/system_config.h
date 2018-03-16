/*
 * system_config.h
 *
 *  Created on: Oct 20, 2016
 *      Author: Bumsik Kim
 */

#ifndef SYSTEM_CONFIG_H_
#define SYSTEM_CONFIG_H_

#ifdef __cplusplus
extern "C"{
#endif

void Error_Handler(void);
void system_init(void);
void peripheral_init(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_CONFIG_H_ */
