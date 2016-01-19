/*
 * timer.c
 *
 *  Created on: Jan 7, 2016
 *      Author: Bumsik Kim
 */


#include "timer.h"

static uint32_t uwMillis;
static uint32_t uwMicros;

/**
 * @brief Initialize Timer
 */
void vTimerInit(void)
{
  uwMillis = 0;
  uwMicros = 0;
}

/**
 * @brief Increase ::uwMillis. Should be call every milliseconds.
 */
void vTimerIncMillis(void)
{
  uwMillis++;
}

/**
 * @brief get current time in milliseconds
 * @return current time in milliseconds.
 */
uint32_t uwTimerGetMillis(void)
{
  return uwMillis;
}

/**
 * @brief get current time in microseconds
 * @return current time in microseconds
 */
uint32_t uwTimerGetMicros(void)
{
  /* TODO: must be shorten by getting SystemCoreClock */
  uwMicros = uwMillis*1000 + (SystemCoreClock/1000 - SysTick->VAL)/confMCU_CLOCK_MHZ;
  return uwMicros;
}

/**
 * @brief Delay system in Milliseconds
 * @param uwInput   time to delay in milliseconds
 */
void vTimerDelayMillis(uint32_t uwInput)
{
  uint32_t uwCurrent = uwMillis;
  while (uwInput - (uwMillis - uwCurrent) > 0);
}

/**
 * @brief Delay system in microseconds
 * @param uwInput   time to delay in microseconds
 * @warning Might not accurate depends on compiler code compression rate and clock rate of the MCU.
 */
void vTimerDelayMicros(uint32_t uwInput)
{
  uint32_t uwCurrent = uwTimerGetMicros();
  while (uwInput - (uwTimerGetMicros() - uwCurrent) > 0);
}
