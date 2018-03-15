/*
 * nm_bsp_kb_lib.c
 *
 *  Created on: Nov 17, 2016
 *      Author: Bumsik Kim
 */


/**
 *
 * \file
 *
 * \brief This module contains SAMD21 BSP APIs implementation.
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
#include "../../conf_winc.h"
#include "kb_common_source.h"   // TODO: include it in conf_winc.h
#include "kb_gpio.h"
#include "kb_tick.h"

/*
 *  @fn     init_chip_pins
 *  @brief  Initialize reset, chip enable and wake pin
 */
static void init_chip_pins(void)
{
    /* Configure control pins as output. */
    kb_gpio_init_t gpio_setting = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = NOPULL,
        .Speed = GPIO_SPEED_FREQ_VERY_HIGH // 50MHz
    };
    kb_gpio_init(WINC_RESET_PORT, WINC_RESET_PIN, &gpio_setting);
    kb_gpio_init(WINC_WAKE_PORT, WINC_WAKE_PIN, &gpio_setting);
    kb_gpio_init(WINC_CHIP_ENABLE_PORT, WINC_CHIP_ENABLE_PIN, &gpio_setting);
}

/*
 *  @fn     nm_bsp_init
 *  @brief  Initialize BSP
 *  @return 0 in case of success and -1 in case of failure
 */
sint8 nm_bsp_init(void)
{
    /* Clear ISR function pointer */
    kb_gpio_isr_delete(WINC_INT_PORT, WINC_INT_PIN);

    /* Initialize chip IOs. */
    init_chip_pins();

        /* TODO: Set timer? */
    /* Make sure a 1ms Systick is configured. */
    if (!(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk && SysTick->CTRL & SysTick_CTRL_TICKINT_Msk)) {
        KB_DEBUG_ERROR("Tick timer is not enabled!");
        return -1;
    }

    /* Perform chip reset. */
    nm_bsp_reset();

    /* Enable glabal interrupt */
    // Nothing needed

    return M2M_SUCCESS;
}

/**
 *  @fn     nm_bsp_deinit
 *  @brief  De-iInitialize BSP
 *  @return 0 in case of success and -1 in case of failure
 */
sint8 nm_bsp_deinit(void)
{
    /* Configure control pins as input no pull up. */

    return M2M_SUCCESS;
}

/**
 *  @fn     nm_bsp_reset
 *  @brief  Reset NMC1500 SoC by setting CHIP_EN and RESET_N signals low,
 *           CHIP_EN high then RESET_N high
 */
void nm_bsp_reset(void)
{
    kb_gpio_set(WINC_CHIP_ENABLE_PORT, WINC_CHIP_ENABLE_PIN, GPIO_PIN_RESET);
    kb_gpio_set(WINC_RESET_PORT, WINC_RESET_PIN, GPIO_PIN_RESET);
    nm_bsp_sleep(100);
    kb_gpio_set(WINC_CHIP_ENABLE_PORT, WINC_CHIP_ENABLE_PIN, GPIO_PIN_SET);
    nm_bsp_sleep(10);
    kb_gpio_set(WINC_RESET_PORT, WINC_RESET_PIN, GPIO_PIN_SET);
    nm_bsp_sleep(100);
}

/*
 *  @fn     nm_bsp_sleep
 *  @brief  Sleep in units of mSec
 *  @param[IN]  u32TimeMsec
 *              Time in milliseconds
 */
void nm_bsp_sleep(uint32 u32TimeMsec)
{
    kb_delay_ms(u32TimeMsec);
}

/*
 *  @fn     nm_bsp_register_isr
 *  @brief  Register interrupt service routine
 *  @param[IN]  pfIsr
 *              Pointer to ISR handler
 */
void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
    /* Register function pointer for ISR */
    kb_gpio_init_t gpio_setting = {
        .Pull = PULLUP,
        .Speed = GPIO_SPEED_FREQ_VERY_HIGH // 50MHz
    };
    kb_gpio_isr_register(WINC_RESET_PORT, WINC_RESET_PIN, pfIsr);
}

/*
 *  @fn     nm_bsp_interrupt_ctrl
 *  @brief  Enable/Disable interrupts
 *  @param[IN]  u8Enable
 *              '0' disable interrupts. '1' enable interrupts
 */
void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{
    if (u8Enable)
    {
        /* Initialize interrupt GPIO */
        /* Trigger mode is falling edge */
        kb_gpio_init_t gpio_setting = {
            .Pull = PULLUP,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH // 50MHz
        };
        kb_gpio_isr_enable(WINC_RESET_PORT, WINC_RESET_PIN, &gpio_setting, FALLING_EDGE);
    }
    else
    {
        kb_gpio_isr_disable(WINC_RESET_PORT, WINC_RESET_PIN);
    }
}
