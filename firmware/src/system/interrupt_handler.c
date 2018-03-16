/*
 * exception_handler.c
 *
 *  Created on: Oct 20, 2016
 *      Author: Bumsik Kim
 */

#include "kb_common_source.h"
#include "interrupt_handler.h"
#include "kb_tick.h"
#include "faults.h"

#ifndef KB_USE_FREERTOS // Learn how to combine this with FreeRTOS

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

void DebugMon_Handler (void)
{
    __DEBUG_BKPT();
    while (1)
    {
    }
}

#endif

void PendSV_Handler (void)
{
    __DEBUG_BKPT();
    while (1)
    {
    }
}

void SVC_Handler (void)
{
    __DEBUG_BKPT();
    while (1)
    {
    }
}

#endif

#ifdef KB_USE_FREERTOS
    #include "FreeRTOS.h"
    #include "task.h"
    extern void xPortSysTickHandler(void);
#endif

extern void SysTick_hook(void); // Can be found in kb_hooks.c

void SysTick_Handler (void)
{
    SysTick_hook();
	kb_tick_inc_ms();

#ifdef KB_USE_FREERTOS
	// FreeRTOS Tick handler
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
#endif
}
