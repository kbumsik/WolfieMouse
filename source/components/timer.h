/*
 * timer.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Bumsik Kim
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "config.h"
#include "stm32f4xx_hal.h"

#ifndef confMCU_CLOCK_MHZ
  #define confMCU_CLOCK_MHZ 100
#endif

#ifdef __cplusplus
extern "C" {
#endif

void timer_init(void);
void timer_inc_mil(void);
uint32_t timer_mil(void);
uint32_t timer_micro(void);
void timer_delay_mil(uint32_t uwInput);
void timer_delay_micro(uint32_t uwInput);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */
