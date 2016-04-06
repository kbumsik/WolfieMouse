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
 * $Date: 2015-07-06 14:00:15 +0200 (Mon, 06 Jul 2015) $
 * $Revision: 2425 $
 */
/**
 * @file vl6180x_freeruning_ranging.c
 *
 * @brief Example of free-running ranging operation
 */

#include "vl6180x_api.h"
#include "vl6180x_sample_plat.h" /* contain all device/platform specific code */

/**
 * @brief  Simple free running ranging
 *
 * Demonstrate how to integrate on-the-fly range check and value pick up in context
 * of a master process running at its own speed without polling or being blocked.
 *
 * that may be your main()
 */
/*! [FreeRunningRanging] */
void Sample_FreeRunningRanging(void) {
    VL6180xDev_t myDev;
    VL6180x_RangeData_t Range;
    int status;

    MyDev_Init(myDev);           // your code init device variable
    MyDev_SetChipEnable(myDev);  // your code assert chip enable
    MyDev_uSleep(1000);          // your code sleep at least 1 msec
    VL6180x_InitData(myDev);
    VL6180x_Prepare(myDev);

    VL6180x_RangeClearInterrupt(myDev); // make sure no interrupt is pending

    /* kick off the first measurement */
    VL6180x_RangeStartSingleShot(myDev);
    do {

        // TODO add your code anything in a loop way
        VL6180x_PollDelay(dev); // simply  run default API poll delay that handle display in demo
        // check for range measure availability
        status= VL6180x_RangeGetMeasurementIfReady(myDev, &Range);
        if( status == 0 ){         
            // Application must check Range.errorStatus before accessing the other data
            //    If Range.errorStatus is DataNotReady, application knows that it has to wait a bit before getting a new data
            //    If Range.errorStatus is 0, application knows it is a valid distance
            //    If Range.errorStatus is not 0, application knows that reported distance is invalid so may take some decisions depending on the errorStatus
            if (Range.errorStatus == DataNotReady)
                continue;
            
            if (Range.errorStatus == 0)
                MyDev_ShowRange(myDev, Range.range_mm,0); // your code display range in mm
            else
                MyDev_ShowErr(myDev, Range.errorStatus); // your code display error code
            /* re-arm next measurement */ 
            VL6180x_RangeStartSingleShot(myDev);   
        }
        else{
            // it is an critical error
            HandleError("critical error on VL6180x_RangeCheckAndGetMeasurement");
        }

    } while (!MyDev_UserSayStop(myDev)); // your code to stop looping
}
/*! [FreeRunningRanging] */
