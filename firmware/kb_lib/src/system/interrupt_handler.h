/*
 * exception_handler.h
 *
 *  Created on: Oct 20, 2016
 *      Author: Bumsik Kim
 */

#ifndef INTERRUPT_HANDLER_H_
#define INTERRUPT_HANDLER_H_

#include "kb_common_source.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Exception Handler */

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
    void DebugMon_Handler (void);
#endif
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler (void);

#ifdef __cplusplus
}
#endif

#endif /* INTERRUPT_HANDLER_H_ */
