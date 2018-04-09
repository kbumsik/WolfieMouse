/*
 * config.h
 *
 *  Created on: Nov 13, 2016
 *      Author: Bumsik Kim
 */

#ifndef SYSTEM_KB_CONFIG_H_
#define SYSTEM_KB_CONFIG_H_

/******************************************************************************
 * KB Library setting
 ******************************************************************************/
#define KB_USE_FREERTOS
#define KB_TRACE
#define KB_DEBUG
    #define KB_DEBUG_TO_TERMINAL
    //#define KB_DEBUG_TO_SEMIHOSTING
#define KB_PRINTF_TO_TERMINAL


/******************************************************************************
 * Vendor specific settings. The setting depends on MCU you choose
 ******************************************************************************/
#if defined(STM32F446xx)|defined(STM32F407xx)
    #define USE_FULL_ASSERT // comment it out for production to reduce code size
#endif

#endif /* SYSTEM_KB_CONFIG_H_ */
