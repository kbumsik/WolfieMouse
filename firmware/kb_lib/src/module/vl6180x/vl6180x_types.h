/*******************************************************************************
Copyright © 2014, STMicroelectronics International N.V.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STMicroelectronics nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED. 
IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/
/*
 * $Date: 2015-01-08 14:30:24 +0100 (Thu, 08 Jan 2015) $
 * $Revision: 2039 $
 */
/**
 * @file  vl6180x_types.h
 * @brief vl6180x types definition
 */

#ifndef VL6180x_TYPES_H_
#define VL6180x_TYPES_H_

/** @defgroup porting_type  Basic type definition
 *  @ingroup  porting
 *
 *  @brief  file vl6180x_types.h file holds basic type definitions that may require porting
 *
 *  Contains type that must be defined for the platform\n
 *  When target platform and compiler provide stdint.h and stddef.h, it is enough to include it.\n
 *  If stdint.h is not available review and adapt all signed and unsigned 8/16/32 bits basic types. \n
 *  If stddef.h is not available review and adapt NULL definition .
 */
#include <stdint.h>
#include <stddef.h>

#ifndef NULL
#error "TODO review  NULL definition or add required include "
#endif

#if ! defined(STDINT_H) &&  !defined(_GCC_STDINT_H) &&!defined(__STDINT_DECLS) && !defined(_GCC_WRAP_STDINT_H)

#pragma message("Please review  type definition of STDINT define for your platform and add to list above ")

 /*
  *  target platform do not provide stdint or use a different #define than above
  *  to avoid seeing the message below addapt the #define list above or implement
  *  all type and delete these pragma
  */

/** \ingroup porting_type
 * @{
 */

/** @brief Typedef defining 32 bit unsigned int type.\n
 * The developer should modify this to suit the platform being deployed.
 */
typedef unsigned int uint32_t;

/** @brief Typedef defining 32 bit int type.\n
 * The developer should modify this to suit the platform being deployed.
 */
typedef int int32_t;

/** @brief Typedef defining 16 bit unsigned short type.\n
 * The developer should modify this to suit the platform being deployed.
 */
typedef unsigned short uint16_t;

/** @brief Typedef defining 16 bit short type.\n
 * The developer should modify this to suit the platform being deployed.
 */
typedef short int16_t;

/** @brief Typedef defining 8 bit unsigned char type.\n
 * The developer should modify this to suit the platform being deployed.
 */
typedef unsigned char uint8_t;

/** @brief Typedef defining 8 bit char type.\n
 * The developer should modify this to suit the platform being deployed.
 */
typedef signed char int8_t;

/** @}  */
#endif /* _STDINT_H */

#endif /* VL6180x_TYPES_H_ */
