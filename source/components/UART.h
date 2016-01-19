/*
 * uart.h
 *
 *  Created on: Dec 30, 2015
 *      Author: Bumsik Kim
 */

#ifndef UART_H_
#define UART_H_

/* Includes */
#include "components_common.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_gpio.h"
#include "cmsis_os.h"

/**
 * @defgroup UART settings
 * @brief Pin and Peripheral settings for UART
 * @{
 */
#ifndef uartUARTx
  /* Peripheral number definition */
  #define uartUARTx     USART1
  /* Definition for USARTx clock resources */
  #define uartCLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
  #define uartRX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
  #define uartTX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
  #define uartFORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
  #define uartRELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()
  
  /* Definition for USARTx Pins */
  #define uartTX_PIN                    GPIO_PIN_15
  #define uartTX_GPIO_PORT              GPIOA  
  #define uartTX_AF                     GPIO_AF7_USART1
  #define uartRX_PIN                    GPIO_PIN_10
  #define uartRX_GPIO_PORT              GPIOA 
  #define uartRX_AF                     GPIO_AF7_USART1

  /* Definition for UART Initialization */
  #define uartBAUD_RATE                 115200
  #define uartWORD_LENGTH               UART_WORDLENGTH_8B
  #define uartSTOP_BITS                 UART_STOPBITS_1
  #define uartPARITY                    UART_PARITY_NONE

  /* Definition for UART Interrupt */
  #define uartUSARTx_IRQn               USART1_IRQn
  #define uartUSARTx_IRQHandler         USART1_IRQHandler
#endif
/**
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/* global variables */
extern UART_HandleTypeDef xUARTHandle;
extern QueueHandle_t quUARTReceive;

/* Function prototypes */
eStatus_t eUARTInit(UART_HandleTypeDef* pxUARTHandle, USART_TypeDef* pxUARTx);
eStatus_t eUARTGPIOInit(UART_HandleTypeDef* pxUARTHandle);
void vUARTIRQHandler(UART_HandleTypeDef *pxUARTHandle);

#ifdef __cplusplus
}
#endif

#endif /* UART_H_ */
