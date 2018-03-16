/*
 * terminal.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#ifndef MODULE_KB_TERMINAL_H_
#define MODULE_KB_TERMINAL_H_

/* Includes */
#include <common_source.h>
#include "module_config.h"

#ifndef TERMINAL_UART	// STM32 NUCLEO64 default
	#define TERMINAL_UART			USART2
	#define TERMINAL_BAUD_RATE		9600 // or 115200
	#define TERMINAL_TX_PORT 		GPIOA
	#define TERMINAL_TX_PIN 		GPIO_PIN_2
	#define TERMINAL_RX_PORT 		GPIOA
	#define TERMINAL_RX_PIN 		GPIO_PIN_3
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* global variables */
//extern UART_HandleTypeDef xUARTHandle;
//extern QueueHandle_t quUARTReceive;

extern char terminal_tx_buffer[];
extern char terminal_rx_buffer[];

int terminal_init(void);
int terminal_puts(char *str);
char *terminal_gets(char *str);
#define terminal_printf(format, ... ) do {\
						sprintf(terminal_tx_buffer, format, ##__VA_ARGS__); \
						terminal_puts(terminal_tx_buffer); \
						} while(0)

#ifdef __cplusplus
}
#endif

#endif /* MODULE_KB_TERMINAL_H_ */
