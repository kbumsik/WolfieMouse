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
 * finder.c
 *
 *  Created on: Apr 15, 2016
 *      Author: Bumsik Kim
 */
/**
 * @file vl6180x_simple_ranging.c
 *
 * @brief simplest ranging operation
 */

#include "range_finder.h"
#include "vl6180x_api.h"
#include "vl6180x_sample_plat.h" /* contain all device/platform specific code */
#include "tca9545.h"

/**
 * @brief  Simplest polling range
 * that may be your main()
 */
/************************************************************************
 * VL6180 codes
 */
#ifdef __cplusplus
extern "C"{
#endif

static void MX_I2C1_Init(void);

extern I2C_HandleTypeDef hi2c1; /* located in tca9545.h */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "vl6180x_api.h"
#include "x-nucleo-6180xa1.h"

#ifdef DEBUG
#include "diag/trace.h"
#define debug(msg, ...)   trace_printf(msg,__VA_ARGS__)
#define trace_warn(msg,...) trace_printf("W %s %d" msg "\n", __func__, __LINE__, __VA_ARGS__)
#else
#define debug(msg, ...)  (void)0
#endif

#define DigitDisplay_ms     1 /* ms each digit is kept on */
#define PressBPSwicthTime   1000 /* if user keep bp press more that this mean swicth mode else rooll over use c&se in mode */

void WaitMilliSec(int ms);

/**
 * VL6180x CubeMX F401 i2c porting implementation
 */

#define theVL6180xDev   0x52    // what we use as "API device
#define i2c_bus      (&hi2c1)
#define def_i2c_time_out 100
#if VL6180x_SINGLE_DEVICE_DRIVER
int VL6180x_I2CWrite(VL6180xDev_t addr, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Transmit(i2c_bus, addr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }
    return status;
}

int VL6180x_I2CRead(VL6180xDev_t addr, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Receive(i2c_bus, addr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }

    return status;
}
#else
int VL6180x_I2CWrite(VL6180xDev_t dev, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Transmit(i2c_bus, dev->I2cAddr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }
    return status;
}

int VL6180x_I2CRead(VL6180xDev_t dev, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Receive(i2c_bus, dev->I2cAddr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }

    return status;
}

#endif

/**
 * platform and application specific for VL6180x_Shield
 */
void XNUCLEO6180XA1_WaitMilliSec(int n) {
    WaitMilliSec(n);
}

volatile int IntrFired = 0;

void XNUCLEO6180XA1_UserIntHandler(void) {
    IntrFired++;
}

/**
 * DISPLAY public
 */
/***************  DISPLAY PUBLIC *********************/
const char *DISP_NextString;
/***************  DISPLAY PRIVATE *********************/
static char DISP_CurString[10];
static int DISP_Loop = 0;
uint32_t TimeStarted; /* various display and mode delay starting time */

#ifdef TEMP  /** TODO: Delete */
void DISP_ExecLoopBody(void) {
    if (DISP_NextString != NULL) {
        strncpy(DISP_CurString, DISP_NextString, sizeof(DISP_CurString) - 1);
        DISP_CurString[sizeof(DISP_CurString) - 1] = 0;
        DISP_NextString = NULL;
    }
    XNUCLEO6180XA1_DisplayString(DISP_CurString, DigitDisplay_ms);
    DISP_Loop++;
}
#else
void DISP_ExecLoopBody(void) {
    if (DISP_NextString != NULL) {
        strncpy(DISP_CurString, DISP_NextString, sizeof(DISP_CurString) - 1);
        DISP_CurString[sizeof(DISP_CurString) - 1] = 0;
        DISP_NextString = NULL;
    }
    printf("%.4s\n", DISP_CurString);
    DISP_Loop++;
}
#endif

/**
 * Nucleo board specific
 *
 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    XNUCLEO6180XA1_EXTI_CallBackHandle(GPIO_Pin);
}

#define BSP_BP_PORT GPIOC
#define BSP_BP_PIN  GPIO_PIN_13
int BSP_GetPushButton(void) {
    GPIO_PinState state;
    state = HAL_GPIO_ReadPin(BSP_BP_PORT, BSP_BP_PIN);
    return state;
}

void SetDisplayString(const char *msg) {
    DISP_NextString = msg;
}

void WaitMilliSec(int ms) {
    HAL_Delay(ms); /* it's milli sec  cos we do set systick to 1KHz */
}

/**
 * call in the main loop
 * when running under debugger it enable doing direct vl6180x reg access
 * After  breaking at entrance
 * change  the the local index/data and cmd variable to do what needed
 * reg_cmd -1 wr byte -2wr word -3 wr dword
 * reg_cmd 1 rd byte 2 rd word 3 rd dword
 * step to last statement before return and read variable to get rd result exit
 */
void debug_stuff(void) {
    int reg_cmd = 0;
    static uint32_t reg_data;
    static uint16_t reg_index;

    if (reg_cmd) {
        switch (reg_cmd) {
            case -1:
                VL6180x_WrByte(theVL6180xDev, reg_index, reg_data);
                debug("Wr B 0x%X = %d", reg_index, (int)reg_data);
                break;
            case -2:
                VL6180x_WrWord(theVL6180xDev, reg_index, reg_data);
                debug("Wr W 0x%X = %d", reg_index, (int) reg_data);
                break;

            case -3:
                VL6180x_WrDWord(theVL6180xDev, reg_index, reg_data);
                debug("WrDW 0x%X = %d", reg_index, (int)reg_data);
                break;

            case 1:
                reg_data = 0;
                VL6180x_RdByte(theVL6180xDev, reg_index, (uint8_t*) &reg_data);
                debug("RD B 0x%X = %d", reg_index, (int)reg_data);
                break;
            case 2:
                reg_data = 0;
                VL6180x_RdWord(theVL6180xDev, reg_index, (uint16_t*) &reg_data);
                debug("RD W 0x%X = %d", reg_index, (int)reg_data);
                break;

            case 3:
                VL6180x_RdDWord(theVL6180xDev, reg_index, &reg_data);
                debug("RD DW 0x%X = %d", reg_index, (int)reg_data);
                break;
            default:
                debug("Invalid command %d", reg_cmd);
                /* nothing to do*/
                ;
        }
    }
}

/**
 * When button is already pressed it Wait for user to release it
 * if button remain pressed for given time it return true
 * These is used to detect mode switch by long press on blue Push Button
 *
 * As soon as time is elapsed -rb- is displayed  to let user know order
 * the  request to switch mode is taken into account
 *
 * @return True if button remain pressed more than specified time
 */
int PusbButton_WaitUnPress(void) {
    uint32_t TimeStarted = HAL_GetTick();
    uint32_t tick;

    while (!BSP_GetPushButton()) {
        ; /* debounce */
        DISP_ExecLoopBody();
        tick = HAL_GetTick();
        if (-TimeStarted > PressBPSwicthTime) {
            SetDisplayString(" rb ");
        }
    }
    return tick - TimeStarted > PressBPSwicthTime;
}

VL6180xDev_t myDev;

void range_finder_init()
{
	/* MCU Configuration----------------------------------------------------------*/


	    /* Initialize all configured peripherals */
	    MX_I2C1_Init();

	    tca9545_init();
	    tca9545_set(Channel_0);

	    /* these almost just redo what already done just above by CubeMx Init */
	    XNUCLEO6180XA1_GPIO_Init();
	    XNUCLEO6180XA1_I2C1_Init(&hi2c1);

	    tca9545Channel_e channel;

	    for(channel = Channel_0; channel <= Channel_3; channel++)
	    {
	        tca9545_set(channel);
	        MyDev_Init(myDev);           // your code init device variable
	        MyDev_SetChipEnable(myDev);  // your code assert chip enable
	        MyDev_uSleep(1000);          // your code sleep at least 1 msec
	        VL6180x_InitData(myDev);
	        VL6180x_Prepare(myDev);
	    }
}
int range_finder_get(enum range_finder_channel channel)
{
    VL6180x_RangeData_t Range;

	switch (channel)
	{
	case left_side:
		tca9545_set(Channel_0);
		break;
	case right_side:
		tca9545_set(Channel_3);
		break;
	case left_front:
		tca9545_set(Channel_1);
		break;
	case right_front:
		tca9545_set(Channel_2);
		break;
	}
	VL6180x_RangePollMeasurement(myDev, &Range);
	return Range.range_mm;
}

void range_finder_update(int *range)
{
    VL6180x_RangeData_t Range;
    tca9545Channel_e channel;
    channel = Channel_0;
    do {
    	tca9545_set(channel);
        VL6180x_RangePollMeasurement(myDev, &Range);
        if (Range.errorStatus == 0 )
        {
        	*range++ = Range.range_mm;
            MyDev_ShowRange(myDev, Range.range_mm,0); // your code display range in mm
        }
        else
        {
        	*range++ = 255;
            MyDev_ShowErr(myDev, Range.errorStatus); // your code display error code
        }

        channel++;
    } while (channel <= Channel_3); // your code to stop looping
}

void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
  HAL_I2C_Init(&hi2c1);

}

#ifdef __cplusplus
}
#endif
