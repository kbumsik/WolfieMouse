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
 * $Date: 2015-06-18 17:41:54 +0200 (Thu, 18 Jun 2015) $
 * $Revision: 2404 $
 */
 
/**
 * @file vl6180x_range_als_alt_poll.c
 *
 * @brief alternate range and als polling
 */

#include "vl6180x_api.h"
#include "vl6180x_sample_plat.h" /* contain all device/platform specific code */

/**
 * @brief  Sample simple polling alternate range and als measurement
 */
void Sample_AlternateRangeAls(void) {
#if VL6180x_ALS_SUPPORT
    VL6180x_RangeData_t Range;
    VL6180x_AlsData_t Als;
    VL6180xDev_t myDev;
    int n = 0;

    MyDev_Init(myDev);           // your code
    MyDev_SetChipEnable(myDev);  // your code
    MyDev_uSleep(401);           // your code sleep at least 400 micro sec
    VL6180x_WaitDeviceBooted(myDev);
    VL6180x_InitData(myDev);
    //static or pseudo static setting like feature on/off can already be sets
    VL6180x_UpscaleSetScaling(myDev, 2); // set scaling of 2
    VL6180x_Prepare(myDev);

    //apply now all over application settings
    // Disable and set gpio interrupt output pins to Hi-Z
    VL6180x_SetupGPIO1(myDev, CONFIG_GPIO_INTERRUPT_DISABLED, INTR_POL_HIGH);
    // clear any pending/spurious interrupt now that we change mode interrupts polarity
    VL6180x_ClearAllInterrupt(myDev);
    //Run and get Range/ALS alternatively
    do {
        if (n++ < 100) {
            VL6180x_RangePollMeasurement(myDev, &Range);
            if (Range.errorStatus == 0 )
                MyDev_ShowRange(myDev, Range.range_mm,0);
            else
                MyDev_ShowErr(myDev, Range.errorStatus);
        } else {
            VL6180x_AlsPollMeasurement(myDev, &Als);
            MyDev_ShowLux(myDev, Als.lux);
            if (n++ > 150)
                n = 0;
        }
    } while (!MyDev_UserSayStop(myDev));
#endif
}
