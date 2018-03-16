/*
 * terminal.c
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#include "terminal.h"
#include "module_config.h"
#include "uart.h"

char terminal_tx_buffer[160];
char terminal_rx_buffer[160];

int terminal_init(void)
{
    uart_tx_pin(TERMINAL_UART, TERMINAL_TX_PORT, TERMINAL_TX_PIN, NOPULL);
    uart_rx_pin(TERMINAL_UART, TERMINAL_RX_PORT, TERMINAL_RX_PIN, NOPULL);
    return uart_init(TERMINAL_UART, TERMINAL_BAUD_RATE);
}

int terminal_puts(char *str)
{
    return uart_send_str(TERMINAL_UART, str, TIMEOUT_MAX);
}

char *terminal_gets(char *str)
{
    int result = uart_receive(TERMINAL_UART, (uint8_t *)str, 80, TIMEOUT_MAX); // FIXME: more nice gets
    if (result != KB_OK)
    {
        return NULL;
    }
    return str;
}
