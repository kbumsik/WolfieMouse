/*
 * uart.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_UART_H_
#define PERIPHERAL_KB_UART_H_

/* Includes */
#include "common_header.h"
#include "gpio.h"


#if defined(STM32)
    typedef void* uart_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int uart_init(uart_t uart, uint32_t baud_rate);
int uart_tx_pin(uart_t uart, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull);
int uart_rx_pin(uart_t uart, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull);

int uart_send(uart_t uart, uint8_t *buffer, uint16_t size, uint32_t timeout);
int uart_send_str(uart_t uart, char *str, uint32_t timeout);
int uart_receive(uart_t uart, uint8_t *buffer, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_UART_H_ */
