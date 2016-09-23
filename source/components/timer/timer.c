/*
 * timer.c
 *
 *  Created on: Jan 7, 2016
 *      Author: Bumsik Kim
 */


#include "timer.h"

static volatile uint32_t _milis;
static void _set_mil(uint32_t mil);


static void _set_mil(uint32_t mil)
{
	_milis = mil;
}

/**
 * @brief Initialize Timer
 */
void timer_init(void)
{
  _set_mil(0);
}

/**
 * @brief Increase ::uwMillis. Should be call every milliseconds.
 */
void timer_inc_mil(void)
{
  _milis++;
}

/**
 * @brief get current time in milliseconds
 * @return current time in milliseconds.
 */
uint32_t timer_mil(void)
{
  return _milis;
}

/**
 * @brief get current time in microseconds
 * @return current time in microseconds
 */
uint32_t timer_micro(void)
{
	/* TODO: must be shorten by getting SystemCoreClock */
	volatile uint32_t tmp_mil = timer_mil();
	volatile uint32_t micros = tmp_mil*1000 + (SystemCoreClock/1000 - SysTick->VAL)/confMCU_CLOCK_MHZ;
	return micros;
}

/**
 * @brief Delay system in Milliseconds
 * @param uwInput   time to delay in milliseconds
 */
void timer_delay_mil(uint32_t input)
{
  uint32_t current = timer_mil();
  while (input - (timer_mil() - current) > 0);
}

/**
 * @brief Delay system in microseconds
 * @param uwInput   time to delay in microseconds
 * @warning Might not accurate depends on compiler code compression rate and clock rate of the MCU.
 */
void timer_delay_micro(uint32_t input)
{
  uint32_t current = timer_micro();
  while (input - (timer_micro() - current) > 0)
  {
  }
}
