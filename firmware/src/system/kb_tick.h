/*
 * timer.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Bumsik Kim
 */

#ifndef KB_TICK_H_
#define KB_TICK_H_

#include "kb_common_header.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STM32
    /* Generic timer functions */
	void 		kb_tick_update_f_cpu_mhz(void);
	void 		kb_tick_init(void);
	void 		kb_tick_inc_ms(void);
	uint32_t 	kb_tick_ms(void);
	void 		kb_delay_ms(volatile uint32_t delay_ms);
	uint32_t 	kb_tick_us(void);
	void 		kb_delay_us(volatile uint32_t delay_us);
#else
	/* Timer for STM32F4 */
    #include "stm32f4xx_hal.h"
	void 	 kb_tick_update_f_cpu_mhz(void);
	#define	 kb_tick_init()
	#define  kb_tick_inc_ms()       HAL_IncTick()
	#define  kb_tick_ms()           HAL_GetTick()
	#define  kb_delay_ms(delay_ms)  HAL_Delay(delay_ms)
	uint32_t kb_tick_us(void);
	void     kb_delay_us(volatile uint32_t delay_us);
#endif

#ifdef __cplusplus
}
#endif

#endif /* KB_TIMER_H_ */
