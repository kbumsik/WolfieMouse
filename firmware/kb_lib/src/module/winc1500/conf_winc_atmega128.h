#ifndef _CONF_WINC_ATMEGA128_H_
#define _CONF_WINC_ATMEGA128_H_

#include "bsp/include/nm_bsp_mega.h"
#include "gpio.h"

/** Module Pin Settings */
/* ATWINC1500 Xplained Pro (ATWINC1500-MR210PA) Pin configuration
 * 
 * Pin of Xplained Pro  /  Function
 *          5               RESET_N
 *          6               WAKE
 *          9               IRQ_N
 *          10              CHIP_EN
 *          15              SPI_SSN
 *          16              SPI_MOSI
 *          17              SPI_MISO
 *          18              SPI_SCK
 */

/** CPU speed setting (for _delay_ms() function in util/delay.h */
/** The maximal possible delay is 262.14 ms / F_CPU in MHz. */
/** So, in 16MHz the maximum delay is 16ms */
# define F_CPU 16000000UL

/** BUS Type Settings */
#define CONF_WIFI_M2M_SPI                       (1)
#define CONF_WINC_USE_SPI                       (1)

/** SPI Settings */
//#define confWINC_SPI_MODULE                0
#define confWINC_SPI_CS_PORT               gpioPORTB /**< for SPI_SSN pin */
#define confWINC_SPI_CS_PIN                gpioPIN0  /**< for SPI_SSN pin */
#define confWINC_SPI_SCK_PORT              gpioPORTB /**< for SPI_SCK pin */
#define confWINC_SPI_SCK_PIN               gpioPIN1  /**< for SPI_SCK pin */
#define confWINC_SPI_MOSI_PORT             gpioPORTB /**< for SPI_MOSI pin */
#define confWINC_SPI_MOSI_PIN              gpioPIN2  /**< for SPI_MOSI pin */
#define confWINC_SPI_MISO_PORT             gpioPORTB /**< for SPI_MISO pin */
#define confWINC_SPI_MISO_PIN              gpioPIN3  /**< for SPI_MISO pin */

#define confWINC_SPI_SPEED                 spiSpeed_DIV2
#define confWINC_SPI_SAMPLING              spiSAMPLE_LEADING_RISING

/* GPIO Pin Settings */
#define confWINC_RESET_PORT                gpioPORTB  /**< for RESET_N pin */
#define confWINC_RESET_PIN                 gpioPIN4   /**< for RESET_N pin */
#define confWINC_WAKE_PORT                 gpioPORTB  /**< for WAKE    pin */
#define confWINC_WAKE_PIN                  gpioPIN5   /**< for WAKE    pin */
#define confWINC_CHIP_ENABLE_PORT          gpioPORTB  /**< for CHIP_EN pin */
#define confWINC_CHIP_ENABLE_PIN           gpioPIN6   /**< for CHIP_EN pin */

/* IRQ_N Pin Settings */
#define confWINC_INT 					   INT6
#define confWINC_INT_PORT                  gpioPORTE /* INT4 PIN */
#define confWINC_INT_PIN                   gpioPIN6  /* INT4 PIN */
#define confWINC_INT_vect                  INT6_vect

/* USART Pin settings */
#define confWINC_USART_TX_PORT			   gpioPORTE
#define confWINC_USART_TX_PIN			   gpioPIN1
#define confWINC_USART_RX_PORT             gpioPORTE
#define confWINC_USART_RX_PIN              gpioPIN0

/** INTERRUPT Detect Mode Settings */
#define CONF_WIFI_M2M_EDGE_INTERRUPT            (1)

#endif /* _CONF_WINC_ATMEGA128_H_ */