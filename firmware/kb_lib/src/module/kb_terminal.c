/*
 * kb_terminal.c
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#include "kb_terminal.h"
#include "kb_module_config.h"
#include "kb_uart.h"

char kb_terminal_tx_buffer[160];
char kb_terminal_rx_buffer[160];

int kb_terminal_init(void)
{
    kb_uart_tx_pin(TERMINAL_UART, TERMINAL_TX_PORT, TERMINAL_TX_PIN, NOPULL);
    kb_uart_rx_pin(TERMINAL_UART, TERMINAL_RX_PORT, TERMINAL_RX_PIN, NOPULL);
    return kb_uart_init(TERMINAL_UART, TERMINAL_BAUD_RATE);
}

int kb_terminal_puts(char *str)
{
    return kb_uart_send_str(TERMINAL_UART, str, TIMEOUT_MAX);
}

char *kb_terminal_gets(char *str)
{
    int result = kb_uart_receive(TERMINAL_UART, (uint8_t *)str, 80, TIMEOUT_MAX); // FIXME: more nice gets
    if (result != KB_OK)
    {
        return NULL;
    }
    return str;
}
