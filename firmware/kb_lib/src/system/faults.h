/*
 * fault_handler.h
 *
 *  Created on: Oct 20, 2016
 *      Author: Bumsik Kim
 */

#ifndef FAULTS_H_
#define FAULTS_H_

#include "kb_common_source.h"
#include <stdint.h>

#if defined(KB_DEBUG)
    #define __DEBUG_BKPT()  __BKPT(0)
#else
    #define __DEBUG_BKPT()
#endif

// ----------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C"
{
#endif

void enable_faults(void);
int is_debug_session(void);

void NMI_Handler(void);
void HardFault_Handler(void);

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
#endif

  // Exception Stack Frame of the Cortex-M3 or Cortex-M4 processor.
  typedef struct
  {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
#if  defined(__ARM_ARCH_7EM__)
    uint32_t s[16];
#endif
  } ExceptionStackFrame;

#if defined(KB_TRACE)
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

  void dumpExceptionStack (ExceptionStackFrame* frame, uint32_t cfsr, uint32_t mmfar,
                      uint32_t bfar, uint32_t lr);
#endif // defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#endif // defined(TRACE)

void HardFault_Handler_C (ExceptionStackFrame* frame, uint32_t lr);

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
  void UsageFault_Handler_C (ExceptionStackFrame* frame, uint32_t lr);
  void BusFault_Handler_C (ExceptionStackFrame* frame, uint32_t lr);
#endif // defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

#if defined(__cplusplus)
}
#endif



#endif /* FAULTS_H_ */
