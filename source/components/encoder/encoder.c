/*
 * encoder.c
 *
 *  Created on: Apr 15, 2016
 *      Author: Bumsik Kim
 */


#include "stm32f4xx_hal.h"
#include "encoder.h"

//PB7	TIM4_CH1	Encoder_R_CHA
//PB6	TIM4_CH2	Encoder_R_CHB

//PC7	TIM3_CH1	Encoder_L_CHA
//PC6	TIM3_CH2	Encoder_L_CHB

static void encoder_gpio_init(void);

/* Timer handler declaration */
TIM_HandleTypeDef    Encoder_Handle;


/* Timer Encoder Configuration Structure declaration */
TIM_Encoder_InitTypeDef sEncoderConfig;

void encoder_init(void)
{
	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;
	encoder_gpio_init();

	/* Enable TIM clock */
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();

	Encoder_Handle.Instance = TIM4;

	Encoder_Handle.Init.Period            = 65535; //0xFFFF
	Encoder_Handle.Init.Prescaler         = 0;
	Encoder_Handle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	Encoder_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	Encoder_Handle.Init.RepetitionCounter = 0;

	sEncoderConfig.EncoderMode        = TIM_ENCODERMODE_TI12;

	sEncoderConfig.IC1Polarity        = TIM_ICPOLARITY_RISING; //TIM_ICPOLARITY_RISING;
	sEncoderConfig.IC1Selection       = TIM_ICSELECTION_DIRECTTI;
	sEncoderConfig.IC1Prescaler       = TIM_ICPSC_DIV1;
	sEncoderConfig.IC1Filter          = 0;

	sEncoderConfig.IC2Polarity        = TIM_ICPOLARITY_FALLING; //TIM_ICPOLARITY_RISING;
	sEncoderConfig.IC2Selection       = TIM_ICSELECTION_DIRECTTI;
	sEncoderConfig.IC2Prescaler       = TIM_ICPSC_DIV1;
	sEncoderConfig.IC2Filter          = 0;

	if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK)
	{
	  /* Initialization Error */
	  //Error_Handler();
	}
	/* Start the encoder interface */
	  HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);

	Encoder_Handle.Instance = TIM3;

		Encoder_Handle.Init.Period            = 65535; //0xFFFF
		Encoder_Handle.Init.Prescaler         = 0;
		Encoder_Handle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
		Encoder_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
		Encoder_Handle.Init.RepetitionCounter = 0;

		sEncoderConfig.EncoderMode        = TIM_ENCODERMODE_TI12;

		sEncoderConfig.IC1Polarity        = TIM_ICPOLARITY_RISING; //TIM_ICPOLARITY_RISING;
		sEncoderConfig.IC1Selection       = TIM_ICSELECTION_DIRECTTI;
		sEncoderConfig.IC1Prescaler       = TIM_ICPSC_DIV1;
		sEncoderConfig.IC1Filter          = 0;

		sEncoderConfig.IC2Polarity        = TIM_ICPOLARITY_FALLING; //TIM_ICPOLARITY_RISING;
		sEncoderConfig.IC2Selection       = TIM_ICSELECTION_DIRECTTI;
		sEncoderConfig.IC2Prescaler       = TIM_ICPSC_DIV1;
		sEncoderConfig.IC2Filter          = 0;

		if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK)
		{
		  /* Initialization Error */
		  //Error_Handler();
		}
		/* Start the encoder interface */
		  HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);
}

int32_t getRightEncCount(void) {
	//return TIM_GetCounter(TIM5);
	return TIM4->CNT;
}

void resetRightEncCount(void) {
	TIM4->CNT = 0;
}

int32_t getLeftEncCount(void) {
	//return TIM_GetCounter(TIM2);
	return TIM3->CNT;
}

void resetLeftEncCount(void) {
	TIM3->CNT = 0;
}

static void encoder_gpio_init()
{

	//PB7	TIM4_CH1	Encoder_R_CHA
	//PB6	TIM4_CH2	Encoder_R_CHB

	//PC7	TIM3_CH1	Encoder_L_CHA
	//PC6	TIM3_CH2	Encoder_L_CHB
	GPIO_InitTypeDef   GPIO_InitStruct;


	/* Enable TIM GPIOs Clock */
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();

	/* in push-pull, alternate function mode */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* in push-pull, alternate function mode */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
