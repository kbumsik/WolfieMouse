/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define EMITTER_FR_Pin GPIO_PIN_13
#define EMITTER_FR_GPIO_Port GPIOC
#define EMITTER_R_Pin GPIO_PIN_14
#define EMITTER_R_GPIO_Port GPIOC
#define RECV_FR_Pin GPIO_PIN_0
#define RECV_FR_GPIO_Port GPIOC
#define RECV_R_Pin GPIO_PIN_1
#define RECV_R_GPIO_Port GPIOC
#define RECV_L_Pin GPIO_PIN_2
#define RECV_L_GPIO_Port GPIOC
#define RECV_FL_Pin GPIO_PIN_3
#define RECV_FL_GPIO_Port GPIOC
#define EMITTER_L_Pin GPIO_PIN_0
#define EMITTER_L_GPIO_Port GPIOA
#define EMITTER_FL_Pin GPIO_PIN_1
#define EMITTER_FL_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define disPIN_CE_BAR_Pin GPIO_PIN_4
#define disPIN_CE_BAR_GPIO_Port GPIOA
#define disPIN_SCK_Pin GPIO_PIN_5
#define disPIN_SCK_GPIO_Port GPIOA
#define BATT_LEVEL_Pin GPIO_PIN_6
#define BATT_LEVEL_GPIO_Port GPIOA
#define disPIN_MOSI_Pin GPIO_PIN_7
#define disPIN_MOSI_GPIO_Port GPIOA
#define disPIN_RS_Pin GPIO_PIN_4
#define disPIN_RS_GPIO_Port GPIOC
#define disPIN_RESET_BAR_Pin GPIO_PIN_5
#define disPIN_RESET_BAR_GPIO_Port GPIOC
#define LED1_PIN_Pin GPIO_PIN_0
#define LED1_PIN_GPIO_Port GPIOB
#define LED2_PIN_Pin GPIO_PIN_1
#define LED2_PIN_GPIO_Port GPIOB
#define usdPIN_SS_BAR_Pin GPIO_PIN_12
#define usdPIN_SS_BAR_GPIO_Port GPIOB
#define usdPIN_SCK_Pin GPIO_PIN_13
#define usdPIN_SCK_GPIO_Port GPIOB
#define usdPIN_MISO_Pin GPIO_PIN_14
#define usdPIN_MISO_GPIO_Port GPIOB
#define usdPIN_MOSI_Pin GPIO_PIN_15
#define usdPIN_MOSI_GPIO_Port GPIOB
#define counterPIN_L_CHA_Pin GPIO_PIN_6
#define counterPIN_L_CHA_GPIO_Port GPIOC
#define counterPIN_L_CHB_Pin GPIO_PIN_7
#define counterPIN_L_CHB_GPIO_Port GPIOC
#define motorPIN_L_FAULT_BAR_Pin GPIO_PIN_8
#define motorPIN_L_FAULT_BAR_GPIO_Port GPIOC
#define motorPIN_L_DIR_Pin GPIO_PIN_9
#define motorPIN_L_DIR_GPIO_Port GPIOC
#define motorPIN_L_PWM_Pin GPIO_PIN_8
#define motorPIN_L_PWM_GPIO_Port GPIOA
#define motorPIN_R_FAULT_BAR_Pin GPIO_PIN_9
#define motorPIN_R_FAULT_BAR_GPIO_Port GPIOA
#define motorPIN_R_DIR_Pin GPIO_PIN_10
#define motorPIN_R_DIR_GPIO_Port GPIOA
#define motorPIN_R_PWM_Pin GPIO_PIN_11
#define motorPIN_R_PWM_GPIO_Port GPIOA
#define PB2_PIN_Pin GPIO_PIN_12
#define PB2_PIN_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define PB1_PIN_Pin GPIO_PIN_15
#define PB1_PIN_GPIO_Port GPIOA
#define LED4_PIN_Pin GPIO_PIN_10
#define LED4_PIN_GPIO_Port GPIOC
#define LED3_PIN_Pin GPIO_PIN_11
#define LED3_PIN_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define counterPIN_R_CHA_Pin GPIO_PIN_4
#define counterPIN_R_CHA_GPIO_Port GPIOB
#define counterPIN_R_CHB_Pin GPIO_PIN_5
#define counterPIN_R_CHB_GPIO_Port GPIOB
#define imu_INT_Pin GPIO_PIN_7
#define imu_INT_GPIO_Port GPIOB
#define imuPIN_SCL_Pin GPIO_PIN_8
#define imuPIN_SCL_GPIO_Port GPIOB
#define imuPIN_SDA_Pin GPIO_PIN_9
#define imuPIN_SDA_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
 #define USE_FULL_ASSERT    1U 

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
