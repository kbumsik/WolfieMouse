/*
 * kb_include_common.h
 *
 *  Created on: Nov 13, 2016
 *      Author: Bumsik Kim
 */

#ifndef SYSTEM_KB_COMMON_HEADER_H_
#define SYSTEM_KB_COMMON_HEADER_H_

#include "kb_config.h"
#include <stdint.h>

#define KB_LIBRARY

// base library setting
#if defined(USE_HAL_DRIVER)
    #if defined(STM32F446xx) || defined(STM32F407xx)
        #define STM32F4
        #define STM32
    #endif
#else
    #error "Please define device library in kb_config.h!!"
#endif

typedef enum {
    KB_OK       = 0,
    KB_ERROR    = -1,
    KB_BUSY     = -2,
    KB_TIMEOUT  = -3,
    KB_UNKNOWN  = -4
} kb_status_t;

/*
 * Handler to external (a.k.a manufacturers like ST) library
 * This will be handled in .c source file 
 */
typedef void* kb_extlib_handle_t;

#endif /* SYSTEM_KB_COMMON_HEADER_H_ */
