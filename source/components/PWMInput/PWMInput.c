/*
 * PWMInput_it.c
 *
 *  Created on: Dec 26, 2015
 *      Author: Bumsik Kim
 */

/* Includes ------------------------------------------------------------------*/
#include "PWMInput.h"

/* Private Variables ---------------------------------------------------------*/
static uint32_t ulTIM1RawPeriod;
static uint32_t ulTIM1RawDutyCycle;

static uint32_t ulTIM2RawPeriod;
static uint32_t ulTIM2RawDutyCycle;

static uint32_t ulTIM3RawPeriod;
static uint32_t ulTIM3RawDutyCycle;

static uint32_t ulTIM5RawPeriod;
static uint32_t ulTIM5RawDutyCycle;

status_t ePWMInputInit(TIM_HandleTypeDef* pxTIMHandle, TIM_TypeDef* pxTIMx, uint32_t eChannel)
{
  /* Check the parameters -------------------------------------*/
  assert_param(IS_TIM_CCX_INSTANCE(pxTIMx, eChannel));
  /* Only Channel 1 and Channel 2 allowed */
  if (eChannel != TIM_CHANNEL_1 &&
      eChannel != TIM_CHANNEL_2)
  {
    assert_failed((uint8_t *)__FILE__, __LINE__);
  }

  /* Initialize PWMinput_TIMx peripheral as follows:
         + Period = 0xFFFF
         + Prescaler = 499
         + ClockDivision = 0
         + Counter direction = Up
    */
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_IC_InitTypeDef sConfigIC;

  pxTIMHandle->Instance = pxTIMx;
  pxTIMHandle->Init.Prescaler = 49;
  pxTIMHandle->Init.CounterMode = TIM_COUNTERMODE_UP;
  pxTIMHandle->Init.Period = 0xFFFF;
  pxTIMHandle->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  pxTIMHandle->State = HAL_TIM_STATE_RESET;
  if(pxTIMx == TIM1)
  {
    pxTIMHandle->Init.RepetitionCounter = 0;
  }

  if(HAL_TIM_IC_Init(pxTIMHandle) != HAL_OK)
  {
    /* Initialization Error */
    return STATUS_ERROR;
  }

  /*##-2- Configure the Input Capture channels ###############################*/
  /* Common configuration */
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;

  if (eChannel == TIM_CHANNEL_1)
  {
    /* Configure the Input Capture of channel 1 */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    if(HAL_TIM_IC_ConfigChannel(pxTIMHandle, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
      /* Configuration Error */
      return STATUS_ERROR;
    }

    /* Configure the Input Capture of channel 2 */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    HAL_TIM_IC_ConfigChannel(pxTIMHandle, &sConfigIC, TIM_CHANNEL_2);
    if(HAL_TIM_IC_ConfigChannel(pxTIMHandle, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
    {
      /* Configuration Error */
      return STATUS_ERROR;
    }

    /*##-3- Configure the slave mode ###########################################*/
    /* Select the slave Mode: Reset Mode */
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
    sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
    sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sSlaveConfig.TriggerFilter = 0;
    if(HAL_TIM_SlaveConfigSynchronization(pxTIMHandle, &sSlaveConfig) != HAL_OK)
    {
      /* Configuration Error */
      return STATUS_ERROR;
    }
  }
  else if(eChannel == TIM_CHANNEL_2)
  {
    /* Configure the Input Capture of channel 1 */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    if(HAL_TIM_IC_ConfigChannel(pxTIMHandle, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
      /* Configuration Error */
      return STATUS_ERROR;
    }
  
    /* Configure the Input Capture of channel 2 */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    HAL_TIM_IC_ConfigChannel(pxTIMHandle, &sConfigIC, TIM_CHANNEL_2);
    if(HAL_TIM_IC_ConfigChannel(pxTIMHandle, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
    {
      /* Configuration Error */
      return STATUS_ERROR;
    }
  
    /*##-3- Configure the slave mode ###########################################*/
    /* Select the slave Mode: Reset Mode */
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
    sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
    sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sSlaveConfig.TriggerFilter = 0;
    if(HAL_TIM_SlaveConfigSynchronization(pxTIMHandle, &sSlaveConfig) != HAL_OK)
    {
      /* Configuration Error */
      return STATUS_ERROR;
    }
  }
  return STATUS_OK;
}

status_t ePWMInputStart(TIM_HandleTypeDef* pxTIMHandle)
{
  /*##- Start the Input Capture in interrupt mode ##########################*/
  if(HAL_TIM_IC_Start_IT(pxTIMHandle, TIM_CHANNEL_2) != HAL_OK)
  {
      /* Starting Error */
    return STATUS_ERROR;
  }

    /*##- Start the Input Capture in interrupt mode ##########################*/
  if(HAL_TIM_IC_Start_IT(pxTIMHandle, TIM_CHANNEL_1) != HAL_OK)
  {
      /* Starting Error */
    return STATUS_ERROR;
  }
  return STATUS_OK;
}


uint32_t uwPWMInputPeriod(TIM_HandleTypeDef* pxTIMHandle)
{
  /* TIM 1 */
  if(pxTIMHandle->Instance == TIM1)
  {
    return ulTIM1RawPeriod;
  }
  /* TIM 2 */
  else if(pxTIMHandle->Instance == TIM2)
  {
    return ulTIM2RawPeriod;
  }
  /* TIM 3 */
  else if(pxTIMHandle->Instance == TIM3)
  {
    return ulTIM3RawPeriod;
  }
  /* TIM 5 */
  else if(pxTIMHandle->Instance == TIM5)
  {
    return ulTIM5RawPeriod;
  }
  /* Not matched */
  return STATUS_ERROR;
}
uint32_t uwPWMInputDutyCycle(TIM_HandleTypeDef* pxTIMHandle)
{
  /* TIM 1 */
  if(pxTIMHandle->Instance == TIM1)
  {
    return ulTIM1RawDutyCycle;
  }
  /* TIM 2 */
  else if(pxTIMHandle->Instance == TIM2)
  {
    return ulTIM2RawDutyCycle;
  }
  /* TIM 3 */
  else if(pxTIMHandle->Instance == TIM3)
  {
    return ulTIM3RawDutyCycle;
  }
  /* TIM 5 */
  else if(pxTIMHandle->Instance == TIM5)
  {
    return ulTIM5RawDutyCycle;
  }
  /* Not matched */
  return STATUS_ERROR;
}
/**
  * @brief  Input Capture callback in non blocking mode
  * @param  htim: TIM IC handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *pxTIMHandle)
{
  /* TIM 1 */
  if (pxTIMHandle->Instance == TIM1 &&
    pxTIMHandle->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    /* Get the Input Capture value */
    ulTIM1RawPeriod = HAL_TIM_ReadCapturedValue(pxTIMHandle, TIM_CHANNEL_2);

    if (ulTIM1RawPeriod != 0)
    {
      ulTIM1RawDutyCycle = HAL_TIM_ReadCapturedValue(pxTIMHandle, TIM_CHANNEL_1);
    }
    else
    {
      ulTIM1RawPeriod = 0;
      ulTIM1RawDutyCycle = 0;
    }
  }
  /* TIM 2 */
  else if (pxTIMHandle->Instance == TIM2 &&
    pxTIMHandle->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    /* Get the Input Capture value */
    ulTIM2RawPeriod = HAL_TIM_ReadCapturedValue(pxTIMHandle, TIM_CHANNEL_1);

    if (ulTIM2RawPeriod != 0)
    {
      ulTIM2RawDutyCycle = HAL_TIM_ReadCapturedValue(pxTIMHandle, TIM_CHANNEL_2);
    }
    else
    {
      ulTIM2RawPeriod = 0;
      ulTIM2RawDutyCycle = 0;
    }
  }
  /* TIM 3 */
  else if (pxTIMHandle->Instance == TIM3 &&
    pxTIMHandle->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    /* Get the Input Capture value */
    ulTIM3RawPeriod = HAL_TIM_ReadCapturedValue(pxTIMHandle, TIM_CHANNEL_2);

    if (ulTIM3RawPeriod != 0)
    {
      ulTIM3RawDutyCycle = HAL_TIM_ReadCapturedValue(pxTIMHandle, TIM_CHANNEL_1);
    }
    else
    {
      ulTIM3RawPeriod = 0;
      ulTIM3RawDutyCycle = 0;
    }
  }
  /* TIM 5 */
  else if (pxTIMHandle->Instance == TIM5 &&
    pxTIMHandle->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    /* Get the Input Capture value */
    ulTIM5RawPeriod = HAL_TIM_ReadCapturedValue(pxTIMHandle, TIM_CHANNEL_2);

    if (ulTIM5RawPeriod != 0)
    {
      ulTIM5RawDutyCycle = HAL_TIM_ReadCapturedValue(pxTIMHandle, TIM_CHANNEL_1);
    }
    else
    {
      ulTIM5RawPeriod = 0;
      ulTIM5RawDutyCycle = 0;
    }
  }
}
