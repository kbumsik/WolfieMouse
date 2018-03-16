/*
 * spi.h
 *
 *  Created on: Oct 25, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_SPI_H_
#define PERIPHERAL_KB_SPI_H_

#include "common_header.h"
#include "gpio.h"

#if defined(STM32)
    typedef void* spi_t;
#else
    #error "Please define device driver! " __FILE__ "(e.g. USE_HAL_DRIVER)\n"
#endif

typedef enum{
    LEADING_RISING_EDGE = 0x01,
    LEADING_FALLING_EDGE = 0x02,
    TRAILING_RISING_EDGE = 0x03,
    TRAILING_FALLING_EDGE = 0x04,
}spi_polarity_t;

typedef struct{
    uint32_t	frequency;
    spi_polarity_t polarity;
}spi_init_t;

#ifdef __cplusplus
extern "C"{
#endif

int spi_init(spi_t spi, spi_init_t *settings);
int spi_mosi_pin(spi_t spi, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull);
int spi_miso_pin(spi_t spi, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull);
int spi_sck_pin(spi_t spi, gpio_port_t port, gpio_pin_t pin, gpio_pull_t pull);

int spi_send(spi_t spi, uint8_t* buf, uint16_t size);
int spi_send_timeout(spi_t spi, uint8_t *buf, uint16_t size, uint32_t timeout);
int spi_receive(spi_t spi, uint8_t* buf, uint16_t size);
int spi_receive_timeout(spi_t spi, uint8_t *buf, uint16_t size, uint32_t timeout);
int spi_sendreceive(spi_t spi, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t size);
int spi_sendreceive_timeout(spi_t spi, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_SPI_H_ */
