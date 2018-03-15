/*
 * kb_base.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Bumsik Kim
 */

#ifndef SYSTEM_KB_COMMON_SOURCE_H_
#define SYSTEM_KB_COMMON_SOURCE_H_

#include "kb_config.h"
#include "kb_common_header.h"

#define KB_MSG_BASE "KB_SOMEWHERE"

// base library setting
#ifdef  STM32F4
    #include "stm32f4xx.h"
    #include "stm32f4xx_hal_def.h" // TODO: port this to our library

    // double check
    #ifndef STM32
        #define STM32
    #endif
    #ifndef STM32F4
        #error  "Something wrong with CMSIS file (stm32f4xx.h). define STM32F4 manually"
    #endif
#else
    #error "Please define device library in kb_config.h!!"
#endif

// library message printing
#ifdef KB_DEBUG
    #if defined(KB_DEBUG_TO_TERMINAL)
        #include "kb_terminal.h"
        #define KB_DEBUG_MSG(msg, ...)	kb_terminal_printf("KB_LIB:" KB_MSG_BASE ":%d:" msg, __LINE__ ,##__VA_ARGS__)
        #define KB_DEBUG_WARNING(msg, ...)	kb_terminal_printf("KB_LIB:" KB_MSG_BASE ":%d:Warning:" msg, __LINE__ , ##__VA_ARGS__)
        #define KB_DEBUG_ERROR(msg, ...)	kb_terminal_printf("KB_LIB:" KB_MSG_BASE ":%d:Error: " msg, __LINE__ , ##__VA_ARGS__)
    #elif defined(KB_DEBUG_TO_SEMIHOSTING)
        #include "kb_trace.h"
        #define KB_DEBUG_MSG(msg, ...)	trace_printf("KB_LIB:" KB_MSG_BASE ":%d:" msg, __LINE__ , ##__VA_ARGS__)
        #define KB_DEBUG_WARNING(msg, ...)	trace_printf("KB_LIB:" KB_MSG_BASE ":%d:Warning:" msg, __LINE__ , ##__VA_ARGS__)
        #define KB_DEBUG_ERROR(msg, ...)	trace_printf("KB_LIB:" KB_MSG_BASE ":%d:Error: " msg, __LINE__ , ##__VA_ARGS__)
    #else
        #error "Define either DEBUG_TO_SEMIHOSTING or DEBUG_TO_TERMINAL. If you didn't set up Terminal, you may prefer DEBUG_TO_SEMIHOSTING."
    #endif
#else
    #define KB_DEBUG_MSG(msg, ...)
    #define KB_DEBUG_WARNING(msg, ...)
    #define KB_DEBUG_ERROR(msg, ...)
#endif

// library status return setting
#if defined(STM32F4)
    #define TIMEOUT_MAX		HAL_MAX_DELAY
    #define KB_CONVERT_STATUS(hal_status) do{   \
        switch((HAL_StatusTypeDef)hal_status)  \
        {   \
        case HAL_OK:    \
            hal_status = (kb_status_t)KB_OK; \
            break;  \
        case HAL_ERROR: \
            hal_status = (kb_status_t)KB_ERROR;  \
            break;  \
        case HAL_BUSY:  \
            hal_status = (kb_status_t)KB_BUSY;   \
            break;  \
        case HAL_TIMEOUT:   \
            hal_status = (kb_status_t)KB_TIMEOUT;    \
            break;  \
        default:    \
            hal_status = (kb_status_t)KB_UNKNOWN;   \
            break;  \
        }   \
    }while(0)
#else
#endif

#endif /* SYSTEM_KB_COMMON_SOURCE_H_ */
