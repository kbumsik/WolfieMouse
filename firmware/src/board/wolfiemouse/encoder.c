/*
 * encoder.c
 *
 *  Created on: Nov 6, 2016
 *      Author: Bumsik Kim
 */

#include "encoder.h"
#include "common_source.h"
#include "module_config.h"
#include "config_measurements.h"
#include "rotary_encoder.h"

rotary_encoder_t g_encoder_left;
rotary_encoder_t g_encoder_right;

void encoder_init(void)
{
	// init left channel
	tim_ch_pin(ENCODER_L_TIMER, CH_1, ENCODER_L_CHA_PORT, ENCODER_L_CHA_PIN,
					ENCODER_L_PIN_PULLUP);
	tim_ch_pin(ENCODER_L_TIMER, CH_2, ENCODER_L_CHB_PORT, ENCODER_L_CHB_PIN,
					ENCODER_L_PIN_PULLUP);

	// init right channel
	tim_ch_pin(ENCODER_R_TIMER, CH_1, ENCODER_R_CHA_PORT, ENCODER_R_CHA_PIN,
					ENCODER_R_PIN_PULLUP);
	tim_ch_pin(ENCODER_R_TIMER, CH_2, ENCODER_R_CHB_PORT, ENCODER_R_CHB_PIN,
					ENCODER_R_PIN_PULLUP);

	// init timer
	// left channel
	rotary_encoder_init_t enc_setting = {
			.device = ENCODER_L_TIMER,
			.direction = CCW,
			.prescaler = 0x0U
	};
	rotary_encoder_init(&g_encoder_left, &enc_setting);
	
	// right channel
	enc_setting = (rotary_encoder_init_t){
			.device = ENCODER_R_TIMER,
			.direction = CW,
			.prescaler = 0x0U
	};
	rotary_encoder_init(&g_encoder_right, &enc_setting);

	// start encoders
	rotary_encoder_start(&g_encoder_left);
	rotary_encoder_start(&g_encoder_right);

	encoder_left_reset();
	encoder_right_reset();
}

int32_t encoder_left_count(void)
{
	return rotary_encoder_count(&g_encoder_left);
}

int32_t encoder_right_count(void)
{
	return rotary_encoder_count(&g_encoder_right);
}

void encoder_left_reset(void)
{
	rotary_encoder_set(&g_encoder_left, MEASURE_ENCODER_DEFAULT);
}

void encoder_right_reset(void)
{
	rotary_encoder_set(&g_encoder_right, MEASURE_ENCODER_DEFAULT);
}
