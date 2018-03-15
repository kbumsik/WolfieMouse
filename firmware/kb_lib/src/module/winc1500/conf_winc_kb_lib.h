/*
 * conf_winc_kb_lib.h
 *
 *  Created on: Nov 17, 2016
 *      Author: Bumsik Kim
 */

#ifndef MODULE_WINC1500_CONF_WINC_KB_LIB_H_
#define MODULE_WINC1500_CONF_WINC_KB_LIB_H_


/** BUS Type Settings */
#define CONF_WIFI_M2M_SPI                       (1)
#define CONF_WINC_USE_SPI                       (1)

/** Module Pin Settings */
/* 
 * NUCLEO F446 Pin settings by default 
 *
 * MISO : PB4 
 * MOSI : PB5 
 * SCLK : PB3
 * CS   : PC4
 * RESET : PB15 
 * WAKE  : PB14 
 * CE    : PB13
 */

/** SPI Settings */
#define WINC_SPI                       SPI3
#define WINC_SPI_CS_PORT               GPIOC /**< for SPI_SSN pin */
#define WINC_SPI_CS_PIN                GPIO_PIN_4  /**< for SPI_SSN pin */
#define WINC_SPI_SCK_PORT              GPIOB /**< for SPI_SCK pin */
#define WINC_SPI_SCK_PIN               GPIO_PIN_3  /**< for SPI_SCK pin */
#define WINC_SPI_MOSI_PORT             GPIOB /**< for SPI_MOSI pin */
#define WINC_SPI_MOSI_PIN              GPIO_PIN_5  /**< for SPI_MOSI pin */
#define WINC_SPI_MISO_PORT             GPIOB /**< for SPI_MISO pin */
#define WINC_SPI_MISO_PIN              GPIO_PIN_4  /**< for SPI_MISO pin */

/* GPIO Pin Settings */
/*
 * NUCLEO F446 Pin settings by default 
 */
#define WINC_RESET_PORT                GPIOB  /**< for RESET_N pin */
#define WINC_RESET_PIN                 GPIO_PIN_15   /**< for RESET_N pin */
#define WINC_WAKE_PORT                 GPIOB  /**< for WAKE    pin */
#define WINC_WAKE_PIN                  GPIO_PIN_14   /**< for WAKE    pin */
#define WINC_CHIP_ENABLE_PORT          GPIOB  /**< for CHIP_EN pin */
#define WINC_CHIP_ENABLE_PIN           GPIO_PIN_13   /**< for CHIP_EN pin */

/* IRQ_N Pin Settings */
#define WINC_INT_PORT                  GPIOB /* PCINT20 PIN */
#define WINC_INT_PIN                   GPIO_PIN_1  /* PCINT20 PIN */

/* USART Pin settings */
//#define WINC_USART_TX_PORT             GPIOD
//#define WINC_USART_TX_PIN              GPIO_PIN_1
//#define WINC_USART_RX_PORT             GPIOD
//#define WINC_USART_RX_PIN              GPIO_PIN_0

/** INTERRUPT Detect Mode Settings */
//#define CONF_WIFI_M2M_EDGE_INTERRUPT            (1)

#endif /* MODULE_WINC1500_CONF_WINC_KB_LIB_H_ */
