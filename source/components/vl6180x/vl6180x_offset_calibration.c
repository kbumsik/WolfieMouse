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
 * $Date: 2015-06-15 14:21:08 +0200 (Mon, 15 Jun 2015) $
 * $Revision: 2401 $
 */
 
/**
 * @file vl6180x_offset_calibration.c
 *
 * @brief Offset calibration procedure
 */

#include <vl6180x_api.h>
#include <vl6180x_sample_plat.h> /* contain all device/platform specific code */

#define N_MEASURE_AVG   10


/**
 * All in one device init
 * @param myDev  The device
 * @return     0 on success may @a #CALIBRATION_WARNING <0 on errir
 */
int Sample_InitForOffsetCalib(VL6180xDev_t myDev){
    int status, init_status;
    MyDev_Init(myDev);           // your code
    MyDev_SetChipEnable(myDev);  // your code
    MyDev_uSleep(2000);          // your code sleep at least 1msec prior to do i2c to device
    init_status = VL6180x_InitData(myDev);
    if(init_status == 0 || init_status == CALIBRATION_WARNING ){
        status = VL6180x_Prepare(myDev);
        if( !status )
            status=init_status; // if prepare is successfull return potential init warning
    }
    return status; 
    }

/**
 * Implement Offset calibration as described in VL6180x Datasheet
 *
 * Device must be initialized
 *
 * @note device scaling wrap filter and xtalk setting are scraped !
 *  It is safer to reset and re init device when done
 * @warning  follow strictly procedure described in the device manual
 * @param myDev  The device
 * @return The offset value (signed interger)
 */
int Sample_OffsetRunCalibration(VL6180xDev_t myDev)
{
    VL6180x_RangeData_t Range[N_MEASURE_AVG];
    int32_t   RangeSum;
    int status;
    int i;
    int offset;
    int RealTargetDistance;

    /* Real target distance is 50 mm in proximity ranging configuration (scaling x1) or 100 mm in extended-range configuration */
    RealTargetDistance = (VL6180x_UpscaleGetScaling(myDev)==1) ? 50 : 100;
    

    /* Turn off wrap-around filter (to avoid first invalid distances and decrease number of I2C accesses at maximum) */
    VL6180x_FilterSetState(myDev, 0);

    /* Clear all interrupts */
    status = VL6180x_ClearAllInterrupt(myDev);
    if( status ){
        HandleError("VL6180x_ClearAllInterrupt  fail");
    }

    /* Ask user to place a white target at know RealTargetDistance */
    MyDev_Printf(myDev, "Calibrating : place white target at %dmm",RealTargetDistance);
    
    /* Program a null offset */
    VL6180x_SetOffsetCalibrationData(myDev, 0);
    
    /* Perform several ranging measurement */
    for( i=0; i<N_MEASURE_AVG; i++){
        status = VL6180x_RangePollMeasurement(myDev, &Range[i]);
        if( status ){
            HandleError("VL6180x_RangePollMeasurement  fail");
        }
        if( Range[i].errorStatus != 0 ){
            HandleError("No target detect");
        }
    }
    
    /* Calculate ranging average (sum) */
    RangeSum=0;
    for( i=0; i<N_MEASURE_AVG; i++){
        RangeSum+= Range[i].range_mm;
    }
    
    /* Calculate part-to-part offset */
    offset = RealTargetDistance - (RangeSum/N_MEASURE_AVG);
    MyDev_Printf(myDev, "offset %d", offset);
    return offset;
}


uint8_t Sample_OffsetCalibrate(void) {
    VL6180xDev_t myDev;
    VL6180x_RangeData_t Range;

    int offset;
    int status;

    /* init device */
    MyDev_Init(myDev);
    status = Sample_InitForOffsetCalib(myDev);
    if( status <0 ){
        HandleError("Sample_Init fail");
    }
    /* run calibration */
    offset = Sample_OffsetRunCalibration(myDev);

    /* when possible reset re-init device otherwise set back required filter */
    VL6180x_FilterSetState(myDev, 1);  // turn on wrap around filter again
    
    /* program offset */
    VL6180x_SetOffsetCalibrationData(myDev, offset);
    
    /* Perform ranging measurement to check */
    do {
        VL6180x_RangePollMeasurement(myDev, &Range);
        if (Range.errorStatus == 0 )
            MyDev_ShowRange(myDev, Range.range_mm,0); // your code display range in mm
        else
            MyDev_ShowErr(myDev, Range.errorStatus); // your code display error code
    } while (!MyDev_UserSayStop(myDev)); // your code to stop looping
    return offset;
}
