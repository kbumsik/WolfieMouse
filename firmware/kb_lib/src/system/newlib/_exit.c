//
// This file is part of the µOS++ III distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <kb_trace.h>
#include <stdlib.h>
#include "stm32f4xx.h"

// ----------------------------------------------------------------------------

#if !defined(DEBUG)
extern void Reset_Handler(void);
#endif

// ----------------------------------------------------------------------------

// Forward declaration

void
_exit(int code);

// ----------------------------------------------------------------------------

// On Release, call the hardware reset procedure.
// On Debug we just enter an infinite loop, to be used as landmark when halting
// the debugger.
//
// It can be redefined in the application, if more functionality
// is required.

void
__attribute__((weak))
_exit(int code __attribute__((unused)))
{
#if !defined(DEBUG)
	Reset_Handler();
#endif

	// TODO: write on trace
	if (CoreDebug->DHCSR & 1)	// Check C_DEBUGEN == 1 -> Debugger connected
	{
		__asm__("BKPT");
	}
	while (1);
}

// ----------------------------------------------------------------------------

void
__attribute__((weak,noreturn))
abort(void)
{
  trace_puts("abort(), exiting...");

  _exit(1);
}

// ----------------------------------------------------------------------------
