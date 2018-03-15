/*
 * kb_uart.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_UART_H_
#define PERIPHERAL_KB_UART_H_

/* Includes */
#include "kb_common_header.h"
#include "kb_gpio.h"


#if defined(STM32)
    typedef void* kb_uart_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int kb_uart_init(kb_uart_t uart, uint32_t baud_rate);
int kb_uart_tx_pin(kb_uart_t uart, kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_pull_t pull);
int kb_uart_rx_pin(kb_uart_t uart, kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_pull_t pull);

int kb_uart_send(kb_uart_t uart, uint8_t *buffer, uint16_t size, uint32_t timeout);
int kb_uart_send_str(kb_uart_t uart, char *str, uint32_t timeout);
int kb_uart_receive(kb_uart_t uart, uint8_t *buffer, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_UART_H_ */
