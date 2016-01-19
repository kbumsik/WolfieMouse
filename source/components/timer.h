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

void vTimerInit(void);
void vTimerIncMillis(void);
uint32_t uwTimerGetMillis(void);
uint32_t uwTimerGetMicros(void);
void vTimerDelayMillis(uint32_t uwInput);
void vTimerDelayMicros(uint32_t uwInput);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */
