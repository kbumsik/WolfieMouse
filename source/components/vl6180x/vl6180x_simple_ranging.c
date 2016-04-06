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
 * $Date: 2015-06-11 12:21:44 +0200 (Thu, 11 Jun 2015) $
 * $Revision: 2396 $
 */
 
/**
 * @file vl6180x_simple_ranging.c
 *
 * @brief simplest ranging operation
 */

#include "vl6180x_api.h"
#include "vl6180x_sample_plat.h" /* contain all device/platform specific code */

/**
 * @brief  Simplest polling range
 * that may be your main()
 */
/*! [SimpleRanging] */
void Sample_SimpleRanging(void) {
    VL6180xDev_t myDev;
    VL6180x_RangeData_t Range;

    MyDev_Init(myDev);           // your code init device variable
    MyDev_SetChipEnable(myDev);  // your code assert chip enable
    MyDev_uSleep(1000);          // your code sleep at least 1 msec
    VL6180x_InitData(myDev);
    VL6180x_Prepare(myDev);
    do {
        VL6180x_RangePollMeasurement(myDev, &Range);
        if (Range.errorStatus == 0 )
            MyDev_ShowRange(myDev, Range.range_mm,0); // your code display range in mm
        else
            MyDev_ShowErr(myDev, Range.errorStatus); // your code display error code
    } while (!MyDev_UserSayStop(myDev)); // your code to stop looping
}
/*! [SimpleRanging] */
