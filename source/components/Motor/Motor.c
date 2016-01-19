/*
 * kb_stm32_hal_motor.c
 *
 *  Created on: Aug 1, 2015
 *      Author: BumSik
 */


#include "Motor.h"

/* Private variables ---------------------------------------------------------*/


/**
 * Function Implementations
 */
eStatus_t eMotorInit(TIM_HandleTypeDef* pxTIMHandle)
{
  /* Counter Prescaler value */
  uint32_t uhPrescalerValue;

  /* Enable MOTOR_TIMx Clock */
  MOTOR_TIMx_CLK_ENABLE();

  /* Compute the prescaler value to have TIM3 counter clock equal to 21 MHz */

  /* Initialize MOTOR_TIMx peripheral as follow:
     + Prescaler = ((SystemCoreClock) /50 MHz) - 1 = TIM_PRESCALER
     + Period = PERIOD_VALUE
     + ClockDivision = 0
     + Counter direction = Up
   */
  uhPrescalerValue = (uint32_t)MOTOR_TIM_PRESCALER;
  pxTIMHandle->Instance = MOTOR_TIMx;
  pxTIMHandle->Init.Prescaler = uhPrescalerValue;
  pxTIMHandle->Init.CounterMode = TIM_COUNTERMODE_UP;
  pxTIMHandle->Init.Period = MOTOR_PERIOD_VALUE;
  pxTIMHandle->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  pxTIMHandle->State = HAL_TIM_STATE_RESET;
  if(HAL_TIM_PWM_Init(pxTIMHandle) != HAL_OK)
  {
    /* Initialization Error */
    return STATUS_ERROR;
  }

  /* Firstly set speed 0 */
  swMotorSetSpeed(pxTIMHandle, 0, eMOTOR_CHANNEL_ALL);
  return STATUS_OK;
}

/**
 * @brief      Initialize all configured GPIO Pins
 */
void vMotorGPIOInit(TIM_HandleTypeDef* pxTIMHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  if(pxTIMHandle->Instance == MOTOR_TIMx)
  {
    /* GPIO Ports Clock Enable */
    MOTOR_TIMx_CHANNEL_GPIO_PORT();
  
    /* GPIO Setting for PWM */
    GPIO_InitStruct.Pin = (MOTOR_GPIO_PIN_CHANNEL_1|
                           MOTOR_GPIO_PIN_CHANNEL_2|
                           MOTOR_GPIO_PIN_CHANNEL_3|
                           MOTOR_GPIO_PIN_CHANNEL_4);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = MOTOR_GPIO_ALTERNATE;
    HAL_GPIO_Init(MOTOR_GPIO_PORT, &GPIO_InitStruct);
  }
}

int32_t swMotorSetSpeed(TIM_HandleTypeDef* pxTIMHandle, int32_t swSpeed, eMotorChannel_t eChannel)
{
  /* Timer Output Compare Configuration Structure declaration */
  TIM_OC_InitTypeDef  sConfig;

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  sConfig.OCMode = TIM_OCMODE_PWM1;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;
  sConfig.Pulse = (uint32_t)swSpeed;
  switch(eChannel)
  {	
  case eMOTOR_CHANNEL_1:
    /* Set the pulse value for channel 1 */
    HAL_TIM_PWM_ConfigChannel(pxTIMHandle, &sConfig, TIM_CHANNEL_1);
  case eMOTOR_CHANNEL_2:
    /* Set the pulse value for channel 2 */
    HAL_TIM_PWM_ConfigChannel(pxTIMHandle, &sConfig, TIM_CHANNEL_2);
  break;
  
  case eMOTOR_CHANNEL_3:
    /* Set the pulse value for channel 3 */
    HAL_TIM_PWM_ConfigChannel(pxTIMHandle, &sConfig, TIM_CHANNEL_3);
  break;
  
  case eMOTOR_CHANNEL_4:
    /* Set the pulse value for channel 4 */
    HAL_TIM_PWM_ConfigChannel(pxTIMHandle, &sConfig, TIM_CHANNEL_4);
  break;
  
  case eMOTOR_CHANNEL_ALL:
    /* Set the pulse value for all channel */
    HAL_TIM_PWM_ConfigChannel(pxTIMHandle, &sConfig, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(pxTIMHandle, &sConfig, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(pxTIMHandle, &sConfig, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(pxTIMHandle, &sConfig, TIM_CHANNEL_4);
  break;
  
  default:
    /* Return Error in other cases */
    return 0;
  }

  /* return OK */
  return swSpeed;
}

eStatus_t eMotorStart(TIM_HandleTypeDef* pxTIMHandle, eMotorChannel_t eChannel)
{
  /*##- Start PWM signals generation #######################################*/
  switch(eChannel)
  {
  case eMOTOR_CHANNEL_1:
    /* Start channel 1 */
    if(HAL_TIM_PWM_Start(pxTIMHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case eMOTOR_CHANNEL_2:
    /* Start channel 2 */
    if(HAL_TIM_PWM_Start(pxTIMHandle, TIM_CHANNEL_2) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case eMOTOR_CHANNEL_3:
    /* Start channel 3 */
    if(HAL_TIM_PWM_Start(pxTIMHandle, TIM_CHANNEL_3) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case eMOTOR_CHANNEL_4:
    /* Start channel 4 */
    if(HAL_TIM_PWM_Start(pxTIMHandle, TIM_CHANNEL_4) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case eMOTOR_CHANNEL_ALL:
    /* Start channel 1 */
    if(HAL_TIM_PWM_Start(pxTIMHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
    /* Start channel 2 */
    if(HAL_TIM_PWM_Start(pxTIMHandle, TIM_CHANNEL_2) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
    /* Start channel 3 */
    if(HAL_TIM_PWM_Start(pxTIMHandle, TIM_CHANNEL_3) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
    /* Start channel 4 */
    if(HAL_TIM_PWM_Start(pxTIMHandle, TIM_CHANNEL_4) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;

  default:
    return STATUS_ERROR;
  }
  /* Return OK */
  return STATUS_OK;
}

eStatus_t eMotorStop(TIM_HandleTypeDef* pxTIMHandle, eMotorChannel_t eChannel)
{
  /*##- Stop PWM signals generation #######################################*/
  switch(eChannel)
  {
  case eMOTOR_CHANNEL_1:
    /* Start channel 1 */
    if(HAL_TIM_PWM_Stop(pxTIMHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case eMOTOR_CHANNEL_2:
    /* Start channel 2 */
    if(HAL_TIM_PWM_Stop(pxTIMHandle, TIM_CHANNEL_2) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case eMOTOR_CHANNEL_3:
    /* Start channel 3 */
    if(HAL_TIM_PWM_Stop(pxTIMHandle, TIM_CHANNEL_3) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case eMOTOR_CHANNEL_4:
    /* Start channel 4 */
    if(HAL_TIM_PWM_Stop(pxTIMHandle, TIM_CHANNEL_4) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case eMOTOR_CHANNEL_ALL:
    /* Start channel 1 */
    if(HAL_TIM_PWM_Stop(pxTIMHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
    /* Start channel 2 */
    if(HAL_TIM_PWM_Stop(pxTIMHandle, TIM_CHANNEL_2) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
    /* Start channel 3 */
    if(HAL_TIM_PWM_Stop(pxTIMHandle, TIM_CHANNEL_3) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
    /* Start channel 4 */
    if(HAL_TIM_PWM_Stop(pxTIMHandle, TIM_CHANNEL_4) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;

  default:
    return STATUS_ERROR;
  }
  /* Return OK */
  return STATUS_OK;
}

eStatus_t eMotorDeInit(TIM_HandleTypeDef* pxTIMHandle)
{
  if(HAL_TIM_PWM_DeInit(pxTIMHandle) != HAL_OK)
  {
    /* Initialization Error */
    return STATUS_ERROR;
  }

  /* Enable MOTOR_TIMx Clock */
  MOTOR_TIMx_CLK_DISABLE();

  /* Return OK */
  return STATUS_OK;
}
