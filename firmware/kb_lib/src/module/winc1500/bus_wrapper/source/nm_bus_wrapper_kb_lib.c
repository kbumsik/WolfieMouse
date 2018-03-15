/*
 * nm_bus_wrapper_kb_lib.c
 *
 *  Created on: Nov 18, 2016
 *      Author: Bumsik Kim
 */

/**
 *
 * \file
 *
 * \brief This module contains NMC1000 bus wrapper APIs implementation.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "../../bsp/include/nm_bsp.h"
#include "../../common/include/nm_common.h"
#include "../../bus_wrapper/include/nm_bus_wrapper.h"
#include "../../conf_winc.h"

#include "kb_spi.h"
#include "kb_gpio.h"

#define NM_BUS_MAX_TRX_SZ   256

tstrNmBusCapabilities egstrNmBusCapabilities =
{
    NM_BUS_MAX_TRX_SZ
};

#ifdef CONF_WINC_USE_I2C

struct i2c_master_module i2c_master_instance;
#define SLAVE_ADDRESS 0x60

/** Number of times to try to send packet if failed. */
#define I2C_TIMEOUT 100

static sint8 nm_i2c_write(uint8 *b, uint16 sz)
{
    sint8 result = M2M_SUCCESS;
    uint16_t timeout = 0;

    struct i2c_master_packet packet = {
        .address     = SLAVE_ADDRESS,
        .data_length = sz,
        .data        = b,
    };

    /* Write buffer to slave until success. */
    while (i2c_master_write_packet_wait(&i2c_master_instance, &packet) != STATUS_OK) {
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == I2C_TIMEOUT) {
            break;
        }
    }

    return result;
}

static sint8 nm_i2c_read(uint8 *rb, uint16 sz)
{
    uint16_t timeout = 0;
    sint8 result = M2M_SUCCESS;
    struct i2c_master_packet packet = {
        .address     = SLAVE_ADDRESS,
        .data_length = sz,
        .data        = rb,
    };

    /* Write buffer to slave until success. */
    while (i2c_master_read_packet_wait(&i2c_master_instance, &packet) != STATUS_OK) {
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == I2C_TIMEOUT) {
            break;
        }
    }

    return result;
}

static sint8 nm_i2c_write_special(uint8 *wb1, uint16 sz1, uint8 *wb2, uint16 sz2)
{
    static uint8 tmp[NM_BUS_MAX_TRX_SZ];
    m2m_memcpy(tmp, wb1, sz1);
    m2m_memcpy(&tmp[sz1], wb2, sz2);
    return nm_i2c_write(tmp, sz1+sz2);
}
#endif

#ifdef CONF_WINC_USE_SPI

/**
 * @brief      write (send) data stream
 *
 * @param      pu8Mosi  1-byte MOSI stream, meaning output(to WINC1500) buffer
 * @param      pu8Miso  1-byte MISO stream, meaning input(from WINC1500) buffer
 * @param[in]  u16Sz    number of bytes to transfer
 *
 * @return     ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
static sint8 spi_rw(uint8* pu8Mosi, uint8* pu8Miso, uint16 u16Sz)
{
    uint8 u8Dummy = 0;
    uint8 u8SkipMosi = 0, u8SkipMiso = 0;
    uint16_t txd_data = 0;
    uint16_t rxd_data = 0;

    if (!pu8Mosi) {
        pu8Mosi = &u8Dummy;
        u8SkipMosi = 1;
    }
    else if(!pu8Miso) {
        pu8Miso = &u8Dummy;
        u8SkipMiso = 1;
    }
    else {
        return M2M_ERR_BUS_FAIL;
    }

    /* Select WINC1500 module by enabling SS Pin */
    kb_gpio_set(WINC_SPI_CS_PORT, WINC_SPI_CS_PIN, GPIO_PIN_RESET);

    while (u16Sz) {

        /* Write Start */
        txd_data = *pu8Mosi;
        kb_spi_send(WINC_SPI, (uint8_t*)(&txd_data), 1);
        /* Write End */

        /* Read */
        /* Read SPI master data register. */
        kb_spi_receive(WINC_SPI, (uint8_t*)(&rxd_data), 1);
        *pu8Miso = rxd_data;
        /* Read end */

        u16Sz--;
        if (!u8SkipMiso) {
            pu8Miso++;
        }
        if (!u8SkipMosi) {
            pu8Mosi++;
        }
    }
    /* ignore this code
    while (!spi_is_write_complete(&master))
        ;
    */

    /* Deselect WINC1500 module by deselecting SS pin */
    kb_gpio_set(WINC_SPI_CS_PORT, WINC_SPI_CS_PIN, GPIO_PIN_SET);

    return M2M_SUCCESS;
}
#endif

/*
*   @fn     nm_bus_init
*   @brief  Initialize the bus wrapper
*   @return M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
sint8 nm_bus_init(void *pvinit)
{
    sint8 result = M2M_SUCCESS;

#ifdef CONF_WINC_USE_I2C
    /* Initialize config structure and software module. */
    struct i2c_master_config config_i2c_master;
    i2c_master_get_config_defaults(&config_i2c_master);

    /* Change buffer timeout to something longer. */
    config_i2c_master.buffer_timeout = 1000;

    /* Initialize and enable device with config. */
    i2c_master_init(&i2c_master_instance, SERCOM2, &config_i2c_master);

    i2c_master_enable(&i2c_master_instance);

#elif defined CONF_WINC_USE_SPI
    /* Configure GPIO Pins */
    kb_gpio_init_t gpio_setting = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = NOPULL,
        .Speed = GPIO_SPEED_FREQ_VERY_HIGH // 50MHz
    };
    kb_gpio_init(WINC_SPI_CS_PORT, WINC_SPI_CS_PIN, &gpio_setting);

    kb_spi_sck_pin(WINC_SPI, WINC_SPI_SCK_PORT, WINC_SPI_SCK_PIN, NOPULL);
    kb_spi_mosi_pin(WINC_SPI, WINC_SPI_MOSI_PORT, WINC_SPI_MOSI_PIN, NOPULL);
    kb_spi_miso_pin(WINC_SPI, WINC_SPI_MISO_PORT, WINC_SPI_MISO_PIN, NOPULL);

    kb_gpio_set(WINC_SPI_CS_PORT, WINC_SPI_CS_PIN, GPIO_PIN_SET);
    kb_gpio_set(WINC_SPI_SCK_PORT, WINC_SPI_SCK_PIN, GPIO_PIN_RESET);
    kb_gpio_set(WINC_SPI_MOSI_PORT, WINC_SPI_MOSI_PIN, GPIO_PIN_SET);
    kb_gpio_set(WINC_SPI_MISO_PORT, WINC_SPI_MISO_PIN, GPIO_PIN_RESET);


    /* configure the SPI master and enable. */    
    kb_spi_init_t spi_init = {
            .polarity = TRAILING_RISING_EDGE,   // TODO: CHeck
            .frequency = 12000000   // MAX 48MHz
    };
    kb_spi_init(WINC_SPI, &spi_init);


    nm_bsp_reset();
    nm_bsp_sleep(1);
#endif
    return result;
}

/*
*   @fn     nm_bus_ioctl
*   @brief  send/receive from the bus
*   @param[IN]  u8Cmd
*                   IOCTL command for the operation
*   @param[IN]  pvParameter
*                   Arbitrary parameter depenging on IOCTL
*   @return M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*   @note   For SPI only, it's important to be able to send/receive at the same time
*/
sint8 nm_bus_ioctl(uint8 u8Cmd, void* pvParameter)
{
    sint8 s8Ret = 0;
    switch(u8Cmd)
    {
#ifdef CONF_WINC_USE_I2C
        case NM_BUS_IOCTL_R: {
            tstrNmI2cDefault *pstrParam = (tstrNmI2cDefault *)pvParameter;
            s8Ret = nm_i2c_read(pstrParam->pu8Buf, pstrParam->u16Sz);
        }
        break;
        case NM_BUS_IOCTL_W: {
            tstrNmI2cDefault *pstrParam = (tstrNmI2cDefault *)pvParameter;
            s8Ret = nm_i2c_write(pstrParam->pu8Buf, pstrParam->u16Sz);
        }
        break;
        case NM_BUS_IOCTL_W_SPECIAL: {
            tstrNmI2cSpecial *pstrParam = (tstrNmI2cSpecial *)pvParameter;
            s8Ret = nm_i2c_write_special(pstrParam->pu8Buf1, pstrParam->u16Sz1, pstrParam->pu8Buf2, pstrParam->u16Sz2);
        }
        break;
#elif defined CONF_WINC_USE_SPI
        case NM_BUS_IOCTL_RW: {
            tstrNmSpiRw *pstrParam = (tstrNmSpiRw *)pvParameter;
            s8Ret = spi_rw(pstrParam->pu8InBuf, pstrParam->pu8OutBuf, pstrParam->u16Sz);
        }
        break;
#endif
        default:
            s8Ret = -1;
            M2M_ERR("invalide ioclt cmd\n");
            break;
    }

    return s8Ret;
}

/*
*   @fn     nm_bus_deinit
*   @brief  De-initialize the bus wrapper
*/
sint8 nm_bus_deinit(void)
{
    sint8 result = M2M_SUCCESS;

    /* TODO: disable spi pins */

    /* Configure control pins as input no pull up. */

#ifdef CONF_WINC_USE_I2C
    i2c_master_disable(&i2c_master_instance);
    port_pin_set_config(CONF_WINC_I2C_SCL, &pin_conf);
    port_pin_set_config(CONF_WINC_I2C_SDA, &pin_conf);
#endif /* CONF_WINC_USE_I2C */
#ifdef CONF_WINC_USE_SPI
    // TODO: Disable SPI

#endif /* CONF_WINC_USE_SPI */
    return result;
}

/*
*   @fn         nm_bus_reinit
*   @brief      re-initialize the bus wrapper
*   @param [in] void *config
*                   re-init configuration data
*   @return     M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*   @author     Dina El Sissy
*   @date       19 Sept 2012
*   @version    1.0
*/
sint8 nm_bus_reinit(void* config)
{
    return M2M_SUCCESS;
}

