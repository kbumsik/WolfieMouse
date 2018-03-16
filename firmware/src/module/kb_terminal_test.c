/*
 * kb_terminal.c
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

/* includes -----------------------*/
#include "UART.h"

/* Variables ---------------------*/

/* private */
static uint8_t pcInputBuffer[confUART_RECEIVE_BUFFER_SIZE]; /* input buffer */
static uint8_t ucInputIndex = 0;                            /* input count */

/* Private functions */
void vUARTReceive(UART_HandleTypeDef *pxUARTHandle);

/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
status_t eUARTInit(UART_HandleTypeDef *pxUARTHandle, USART_TypeDef *pxUARTx)
{
    pxUARTHandle->Instance = pxUARTx;
    pxUARTHandle->Init.BaudRate = uartBAUD_RATE;
    pxUARTHandle->Init.WordLength = uartWORD_LENGTH;
    pxUARTHandle->Init.StopBits = uartSTOP_BITS;
    pxUARTHandle->Init.Parity = uartPARITY;
    pxUARTHandle->Init.Mode = UART_MODE_TX_RX;
    pxUARTHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    pxUARTHandle->Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(pxUARTHandle);
    return STATUS_OK;
}

/**
 * @brief Initialize GPIO for UART
 * @param pxUARTHandle
 */
status_t eUARTGPIOInit(UART_HandleTypeDef *pxUARTHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (pxUARTHandle->Instance == uartUARTx)
    {
        /* Peripheral clock enable */
        uartCLK_ENABLE();

        /* GPIO clock enable */
        uartRX_GPIO_CLK_ENABLE();
        uartTX_GPIO_CLK_ENABLE();

        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin = uartTX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = uartTX_AF;
        HAL_GPIO_Init(uartTX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = uartRX_PIN;
        GPIO_InitStruct.Alternate = uartRX_AF;
        HAL_GPIO_Init(uartRX_GPIO_PORT, &GPIO_InitStruct);

        /* Peripheral interrupt init*/
        HAL_NVIC_SetPriority(uartUSARTx_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(uartUSARTx_IRQn);

        /* Enable Interrupts here */
        __HAL_UART_ENABLE_IT(pxUARTHandle, UART_IT_RXNE);
    }
    return STATUS_OK;
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void vUARTIRQHandler(UART_HandleTypeDef *pxUARTHandle)
{
    uint32_t tmp1 = 0, tmp2 = 0;

    tmp1 = __HAL_UART_GET_FLAG(pxUARTHandle, UART_FLAG_RXNE);
    tmp2 = __HAL_UART_GET_IT_SOURCE(pxUARTHandle, UART_IT_RXNE);
    /* UART in mode Receiver ---------------------------------------------------*/
    if ((tmp1 != RESET) && (tmp2 != RESET))
    {
        vUARTReceive(pxUARTHandle);
    }
}

/**
  * @brief  Receives an amount of data in non blocking mode
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
void vUARTReceive(UART_HandleTypeDef *pxUARTHandle)
{
    BaseType_t xHigherPriorityWoken;

    /* copy data */
    pcInputBuffer[ucInputIndex] = (uint8_t)(pxUARTHandle->Instance->DR & (uint8_t)0x00FF);

    /* if the received character is newline */
    if (pcInputBuffer[ucInputIndex] == (uint8_t)'\n')
    {
        /* put null */
        pcInputBuffer[ucInputIndex + 1] = (uint8_t)'\0';
        /* Put in the queue */
        xQueueSendToBackFromISR(quUARTReceive, (void *)pcInputBuffer, &xHigherPriorityWoken);
        /* TODO: what if the Queue is full? */
        ucInputIndex = 0;
        portYIELD_FROM_ISR(xHigherPriorityWoken);
        return;
    }
    /* Increase the buffer index */
    ucInputIndex += 1;
    /* if the index buffer reaches the boundary */
    if (ucInputIndex == confUART_RECEIVE_BUFFER_SIZE - 1)
    {
        /* put null */
        pcInputBuffer[ucInputIndex] = (uint8_t)'\0';
        /* TODO: What if the end of line is without \n? what happened to scanf()? */
        /* Put in the queue */
        xQueueSendToBackFromISR(quUARTReceive, (void *)pcInputBuffer, &xHigherPriorityWoken);
        /* TODO: what if the Queue is full? */
        ucInputIndex = 0;
        portYIELD_FROM_ISR(xHigherPriorityWoken);
        return;
    }
    return;
}
