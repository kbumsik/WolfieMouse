/*******************************************************************************
Copyright © 2015, STMicroelectronics International N.V.
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
 * @file x-nucleo-6180xa1.h
 * $Date: 2015-11-12 14:43:01 +0100 (Thu, 12 Nov 2015) $
 * $Revision: 2619 $
 */

#ifndef XNUCLEO6180XA1_H_
#define XNUCLEO6180XA1_H_


#define SWITCH_VAL_RANGING  0

/* function below must be provided  */
void XNUCLEO6180XA1_WaitMilliSec(int SegDelayMs);
/* optional list */
void XNUCLEO6180XA1_UserIntHandler(void);

/* provided */
void XNUCLEO6180XA1_GPIO_Init(void);
void XNUCLEO6180XA1_I2C1_Init(I2C_HandleTypeDef *hi2c1);
void XNUCLEO6180XA1_DisplayString(const char *str, int SegDelayMs);

void XNUCLEO6180XA1_EnableInterrupt(void);
void XNUCLEO6180XA1_DisableInterrupt(void);
void XNUCLEO6180XA1_ClearInterrupt(void);

int XNUCLEO6180XA1_GetSwitch(void);
void XNUCLEO6180XA1_Reset(int state);

/**
 * @brief  get version of Expansion board
 * @warning to be called only aftrer @a XNUCLEO6180XA1_I2C1_Init() get called
 * @return True if connected shield is v2
 */
int XNUCLEO6180XA1_IsV2(void);

/**
 * @brief  tio be called by HAL EXTI handler dispatecher
 * @warning to be called only aftrer @a XNUCLEO6180XA1_I2C1_Init() get called
 * @param pin
 * @return  non 0 if the GPIO_pin is the GPIO pin for active VL6180x GPIO int line
 */
int XNUCLEO6180XA1_EXTI_CallBackHandle(uint16_t GPIO_Pin);


/*all function below only valid for v2 */
#define XNUCLEO6180XA1_ID_TOP   0
#define XNUCLEO6180XA1_ID_BOT   1
#define XNUCLEO6180XA1_ID_LEFT  2
#define XNUCLEO6180XA1_ID_RIGHT 3
int XNUCLEO6180XA1_ResetId(int state, int id);

#endif /* XNUCLEO6180XA1_H_ */
