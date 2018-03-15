/*
 * kb_spi.h
 *
 *  Created on: Oct 25, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_SPI_H_
#define PERIPHERAL_KB_SPI_H_

#include "kb_common_header.h"
#include "kb_gpio.h"

#if defined(STM32)
    typedef void* kb_spi_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif

typedef enum{
    LEADING_RISING_EDGE = 0x01,
    LEADING_FALLING_EDGE = 0x02,
    TRAILING_RISING_EDGE = 0x03,
    TRAILING_FALLING_EDGE = 0x04,
}kb_spi_polarity_t;

typedef struct{
    uint32_t	frequency;
    kb_spi_polarity_t polarity;
}kb_spi_init_t;

#ifdef __cplusplus
extern "C"{
#endif

int kb_spi_init(kb_spi_t spi, kb_spi_init_t *settings);
int kb_spi_mosi_pin(kb_spi_t spi, kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_pull_t pull);
int kb_spi_miso_pin(kb_spi_t spi, kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_pull_t pull);
int kb_spi_sck_pin(kb_spi_t spi, kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_pull_t pull);

int kb_spi_send(kb_spi_t spi, uint8_t* buf, uint16_t size);
int kb_spi_send_timeout(kb_spi_t spi, uint8_t *buf, uint16_t size, uint32_t timeout);
int kb_spi_receive(kb_spi_t spi, uint8_t* buf, uint16_t size);
int kb_spi_receive_timeout(kb_spi_t spi, uint8_t *buf, uint16_t size, uint32_t timeout);
int kb_spi_sendreceive(kb_spi_t spi, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t size);
int kb_spi_sendreceive_timeout(kb_spi_t spi, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_SPI_H_ */
