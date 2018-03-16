/*
 * tim.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Bumsik Kim
 */

#ifndef KB_TICK_H_
#define KB_TICK_H_

#include "common_header.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STM32
    /* Generic timer functions */
	void 		tick_update_f_cpu_mhz(void);
	void 		tick_init(void);
	void 		tick_inc_ms(void);
	uint32_t 	tick_ms(void);
	void 		delay_ms(volatile uint32_t delay_ms);
	uint32_t 	tick_us(void);
	void 		delay_us(volatile uint32_t delay_us);
#else
	/* Timer for STM32F4 */
    #include "stm32f4xx_hal.h"
	void 	 tick_update_f_cpu_mhz(void);
	#define	 tick_init()
	#define  tick_inc_ms()       HAL_IncTick()
	#define  tick_ms()           HAL_GetTick()
	#define  delay_ms(delay_ms)  HAL_Delay(delay_ms)
	uint32_t tick_us(void);
	void     delay_us(volatile uint32_t delay_us);
#endif

#ifdef __cplusplus
}
#endif

#endif /* KB_TIMER_H_ */
