/*
 * kb_TCA9545A_i2c_mux.c
 *
 *  Created on: Oct 30, 2016
 *      Author: Bumsik Kim
 */

#include "kb_TCA9545A_i2c_mux.h"
#include "kb_i2c.h"
#include "kb_gpio.h"
#include "kb_tick.h"

// base name change. Used with kb_msg(). See @kb_base.h
#ifdef KB_MSG_BASE
	#undef KB_MSG_BASE
	#define KB_MSG_BASE "TCA9545A"
#endif

#define ADDR_		(0x70U)
#define RETRY_		(10)
#define TIMEOUT_	(100)

static inline void reset_(void);
static int write_(uint8_t input);
static uint8_t read_(void);

int tca9545a_init(void)
{
	// SDA, SCL pin
    kb_i2c_sda_pin(TCA9545A_I2C, TCA9545A_SDA_PORT, TCA9545A_SDA_PIN, NOPULL);
    kb_i2c_scl_pin(TCA9545A_I2C, TCA9545A_SCL_PORT, TCA9545A_SCL_PIN, NOPULL);

    // Reset pin
	kb_gpio_init_t reset_pin_settings;
	reset_pin_settings.Mode = GPIO_MODE_OUTPUT_PP;
	reset_pin_settings.Pull = GPIO_PULLUP;
	reset_pin_settings.Speed = GPIO_SPEED_LOW;
    kb_gpio_init(TCA9545A_RESET_PORT, TCA9545A_RESET_PIN, &reset_pin_settings);

    reset_();

	/* Init I2C Bus */
	kb_i2c_init_t setting = {
			.frequency = 400000
	};
	kb_i2c_init(TCA9545A_I2C, &setting);

	/* try reading chip information */
	tca9545a_current_ch();
	return 0;
}


int tca9545a_select_ch(uint8_t ch)
{
	if(ch > 0x0F)
	{
		KB_DEBUG_ERROR("Wrong channel\r\n");
		return -1;
	}
	int result = write_(ch);
	/* FIXME: Delay needed, but what is the minimum? */
	kb_delay_us(100);
	return result;
}


uint8_t tca9545a_current_ch(void)
{
	return	read_() & 0x0F;
}


uint8_t tca9545a_current_it(void)
{
	return	(read_() & 0xF0) >> 4;
}


uint8_t tca9545a_clear_it(uint8_t ch)
{
	// read
	uint8_t value = read_();
	// mask
	uint8_t inturrpts = value & 0xF0;
	inturrpts &= ~(ch<<4);
	value = (value & 0x0F)|inturrpts;
	// and write
	int result = write_(value);
	/* FIXME: Delay needed, but what is the minimum? */
	kb_delay_us(100);
	return result;
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/

static inline void reset_(void)
{
	kb_gpio_set(TCA9545A_RESET_PORT, TCA9545A_RESET_PIN, GPIO_PIN_RESET);
	kb_delay_ms(1);
	kb_gpio_set(TCA9545A_RESET_PORT, TCA9545A_RESET_PIN, GPIO_PIN_SET);
}


static int write_(uint8_t input)
{
	while (KB_OK != kb_i2c_send_timeout(TCA9545A_I2C, ADDR_, &input, 1, TIMEOUT_))
	{
		KB_DEBUG_ERROR("TCA9545 not found! retry\n");
		if (0 == tca9545a_init())
		{
			continue;
		}
		return -1;
	}
	return 0;
}


static uint8_t read_(void)
{
	uint8_t buf = 0;
	uint8_t retry = 0;
	while (KB_OK != kb_i2c_receive_timeout(TCA9545A_I2C, ADDR_, &buf, 1, TIMEOUT_))
	{
		retry++;
		if (retry > RETRY_)
		{
			KB_DEBUG_ERROR("Failed to connect TCA9545!\n");
			return 0;
		}
		KB_DEBUG_ERROR("TCA9545A not found! retry\n");
		reset_();
	}
	return	buf;
}

