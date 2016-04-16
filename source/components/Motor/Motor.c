/*
 * kb_stm32_hal_motor.c
 *
 *  Created on: Aug 1, 2015
 *      Author: BumSik
 */


#include "Motor.h"

/* global variables ---------------------------------------------------------*/
TIM_HandleTypeDef xMotorHandle;

#define motorRIGHT_PORT 		GPIOA
#define motorRIGHT_PIN			GPIO_PIN_10
#define right_set_toggle()		HAL_GPIO_TogglePin(motorRIGHT_PORT, motorRIGHT_PIN)
#define right_set_forward()		HAL_GPIO_WritePin(motorRIGHT_PORT, motorRIGHT_PIN, GPIO_PIN_RESET)
#define right_set_backward()	HAL_GPIO_WritePin(motorRIGHT_PORT, motorRIGHT_PIN, GPIO_PIN_SET)

#define motorLEFT_PORT 			GPIOC
#define motorLEFT_PIN			GPIO_PIN_9
#define left_set_toggle()		HAL_GPIO_TogglePin(motorLEFT_PORT, motorLEFT_PIN)
#define left_set_forward()		HAL_GPIO_WritePin(motorLEFT_PORT, motorLEFT_PIN, GPIO_PIN_SET)
#define left_set_backward()		HAL_GPIO_WritePin(motorLEFT_PORT, motorLEFT_PIN, GPIO_PIN_RESET)
/**
 * Function Implementations
 */
eStatus_t eMotorInit(void)
{
  /* Counter Prescaler value */
  uint32_t uhPrescalerValue;

  /* init GPIO */
  vMotorGPIOInit();
  vMotorGoForward();

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
  xMotorHandle.Instance = MOTOR_TIMx;
  xMotorHandle.Init.Prescaler = uhPrescalerValue;
  xMotorHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  xMotorHandle.Init.Period = MOTOR_PERIOD_VALUE;
  xMotorHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  xMotorHandle.State = HAL_TIM_STATE_RESET;
  if(HAL_TIM_PWM_Init(&xMotorHandle) != HAL_OK)
  {
    /* Initialization Error */
    return STATUS_ERROR;
  }

  /* Firstly set speed 0 */
  swMotorSetSpeed(0, left);
  swMotorSetSpeed(0, right);
  return STATUS_OK;
}

/**
 * @brief      Initialize all configured GPIO Pins
 */
void vMotorGPIOInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  MOTOR_TIMx_CHANNEL_GPIO_PORT();

  /* GPIO Setting for PWM */
  GPIO_InitStruct.Pin = (MOTOR_GPIO_PIN_RIGHT|
                         MOTOR_GPIO_PIN_LEFT);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = MOTOR_GPIO_ALTERNATE;
  HAL_GPIO_Init(MOTOR_GPIO_PORT, &GPIO_InitStruct);

  /****
   * Direction GPIOs
   * * *GPIO Pin Configuration:
   *     left_PWM: PA8
   *     left_dir: PC9
   *     left_fault: PC8
   *     right_PWM: PA11
   *     right_dir: PA10
   *     right_fault: PA9
   */

	/* Enable TIM GPIOs Clock */
	__GPIOA_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	/* in push-pull, alternate function mode */
	GPIO_InitStruct.Pin = (GPIO_PIN_10);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = (GPIO_PIN_9);
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	/* in push-pull, alternate function mode */
	GPIO_InitStruct.Pin = (GPIO_PIN_9);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = (GPIO_PIN_8);
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int32_t swMotorSetSpeed(int32_t swSpeed, eMotorChannel_t eChannel)
{
  /* Timer Output Compare Configuration Structure declaration */
  TIM_OC_InitTypeDef  sConfig;

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  sConfig.OCMode = TIM_OCMODE_PWM1;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;
  sConfig.Pulse = (uint32_t)swSpeed;
  sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  switch(eChannel)
  {	
  case left:
    /* Set the pulse value for channel 1 */
    HAL_TIM_PWM_ConfigChannel(&xMotorHandle, &sConfig, TIM_CHANNEL_1);
  break;

  case right:
    /* Set the pulse value for channel 4 */
    HAL_TIM_PWM_ConfigChannel(&xMotorHandle, &sConfig, TIM_CHANNEL_4);
  break;

  case all:
    /* Set the pulse value for all channel */
    HAL_TIM_PWM_ConfigChannel(&xMotorHandle, &sConfig, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&xMotorHandle, &sConfig, TIM_CHANNEL_4);
  break;
  
  default:
    /* Return Error in other cases */
    return 0;
  }

  /* return OK */
  return swSpeed;
}

void vMotorGoForward(void)
{
	right_set_forward();
	left_set_forward();
}

void vMotorGoBackward(void)
{
	right_set_backward();
	left_set_backward();
}

void vMotorTurnRight(void)
{
	right_set_backward();
	left_set_forward();
}

void vMotorTurnLeft(void)
{
	right_set_forward();
	left_set_backward();
}

eStatus_t eMotorStart(eMotorChannel_t eChannel)
{
  /*##- Start PWM signals generation #######################################*/
  switch(eChannel)
  {
  case left:
    /* Start channel 1 */
    if(HAL_TIM_PWM_Start(&xMotorHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case right:
    /* Start channel 4 */
    if(HAL_TIM_PWM_Start(&xMotorHandle, TIM_CHANNEL_4) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case all:
    /* Start channel 1 */
    if(HAL_TIM_PWM_Start(&xMotorHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
    /* Start channel 4 */
    if(HAL_TIM_PWM_Start(&xMotorHandle, TIM_CHANNEL_4) != HAL_OK)
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

eStatus_t eMotorStop(eMotorChannel_t eChannel)
{
  /*##- Stop PWM signals generation #######################################*/
  switch(eChannel)
  {
  case left:
    /* Start channel 1 */
    if(HAL_TIM_PWM_Stop(&xMotorHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case right:
    /* Start channel 4 */
    if(HAL_TIM_PWM_Stop(&xMotorHandle, TIM_CHANNEL_4) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
  break;
  case all:
    /* Start channel 1 */
    if(HAL_TIM_PWM_Stop(&xMotorHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* PWM Generation Error */
      return STATUS_ERROR;
    }
    /* Start channel 4 */
    if(HAL_TIM_PWM_Stop(&xMotorHandle, TIM_CHANNEL_4) != HAL_OK)
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

eStatus_t eMotorDeInit(void)
{
  if(HAL_TIM_PWM_DeInit(&xMotorHandle) != HAL_OK)
  {
    /* Initialization Error */
    return STATUS_ERROR;
  }

  /* Enable MOTOR_TIMx Clock */
  MOTOR_TIMx_CLK_DISABLE();

  /* Return OK */
  return STATUS_OK;
}
