/*
 * PWMInput.h
 *
 *  Created on: Dec 26, 2015
 *      Author: Bumsik Kim
 */

#ifndef PWMINPUT_H_
#define PWMINPUT_H_

#include "config.h"
#include "PWMInput.h"
#include "stm32f4xx_hal.h"
#include "components_common.h"

/* Global variables */
extern TIM_HandleTypeDef xTIM1Handle;
extern TIM_HandleTypeDef xTIM2Handle;
extern TIM_HandleTypeDef xTIM3Handle;
extern TIM_HandleTypeDef xTIM5Handle;

#ifdef __cplusplus
extern "C"{
#endif

/* Function Prototypes */
status_t ePWMInputInit(TIM_HandleTypeDef* pxTIMHandle, TIM_TypeDef* pxTIMx, uint32_t eChannel);
status_t ePWMInputStart(TIM_HandleTypeDef* pxTIMHandle);
uint32_t uwPWMInputPeriod(TIM_HandleTypeDef* pxTIMHandle);
uint32_t uwPWMInputDutyCycle(TIM_HandleTypeDef* pxTIMHandle);

/* callback functions */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *pxTIMHandle);

#ifdef __cplusplus
}
#endif

#endif /* PWMINPUT_H_ */
