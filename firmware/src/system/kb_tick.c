/*
 * timer.c
 *
 *  Created on: Jan 7, 2016
 *      Author: Bumsik Kim
 */

#include <kb_common_source.h>
#include "kb_tick.h"

static uint16_t f_cpu_mhz_;

#ifndef STM32
static volatile uint32_t ms_;
static void set_ms_(volatile uint32_t ms);


static void set_ms_(volatile uint32_t ms)
{
	ms_ = ms;
}

/**
 * @brief Initialize Timer
 */
void kb_timer_init(void)
{
	set_ms_(0);
}

/**
 * @brief Increase ::uwMillis. Should be call every milliseconds.
 */
void kb_timer_inc_ms(void)
{
	ms_++;
}

/**
 * @brief get current time in milliseconds
 * @return current time in milliseconds.
 */
uint32_t kb_tick_ms(void)
{
	return ms_;
}

/**
 * @brief Delay system in Milliseconds
 * @param uwInput   time to delay in milliseconds
 */
void kb_timer_delay_ms(volatile uint32_t delay_ms)
{
	uint32_t current = kb_tick_ms();
	while ((kb_tick_ms() - current) < delay_ms)
	{
	}
}
#else

/*
 * Overloading original HAL_Delay() in stm32f4xx_hal.c
 */
void HAL_Delay(__IO uint32_t Delay)
{
    /**
     * Added overflow protection on top of the original code.
     */
	uint32_t tickstart = 0U;
	tickstart = HAL_GetTick();
	uint32_t current = tickstart;
	// Overflow detection
	if (((uint32_t)(Delay + tickstart)) < tickstart)
	{	//overflowed
		while (current > Delay)
		{
			current = HAL_GetTick();
		}
		Delay -= UINT32_MAX - tickstart;
		tickstart = 0;
	}
	while((current - tickstart) < Delay)
	{
		current = HAL_GetTick();
	}
	return;
}

#endif

void kb_tick_update_f_cpu_mhz(void)
{
	SystemCoreClockUpdate();
	f_cpu_mhz_ = SystemCoreClock/1000000;
	return;
}
/**
 * @brief get current time in microseconds
 * @return current time in microseconds
 */
uint32_t kb_tick_us(void)
{
	uint32_t tmp_ms;
	volatile uint32_t micros;

	tmp_ms = kb_tick_ms();
	micros = 1000 - SysTick->VAL/f_cpu_mhz_;
	micros += tmp_ms*1000;
	// Millis*1000+(SystemCoreClock/1000-SysTick->VAL)/F_CPU_MHZ;
	return micros;
}

/**
 * @brief Delay system in microseconds
 * @param uwInput   time to delay in microseconds
 * @warning Might not accurate depends on compiler code compression rate and clock rate of the MCU.
 */
void kb_delay_us(volatile uint32_t delay_us)
{
	uint32_t start = kb_tick_us();
	uint32_t current = start;
	// Overflow case detect.
	if (((uint32_t)(delay_us + start)) < start)
	{	//overflowed
		while (current > delay_us)
		{
			current = kb_tick_us();
		}
		delay_us -= UINT32_MAX - start;
		start = 0;
	}
	// FIXME: sometimes the current is lower than start. Why?
	while ((current- start) < delay_us || (current < start))
	{
		current = kb_tick_us();
	}
	return;
}
