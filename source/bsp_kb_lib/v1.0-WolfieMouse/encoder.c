/*
 * encoder.c
 *
 *  Created on: Nov 6, 2016
 *      Author: Bumsik Kim
 */

#include "encoder.h"
#include "kb_common_source.h"
#include "kb_timer.h"
#include "kb_module_config.h"

void encoder_init(void)
{
	// init left channel
	kb_timer_ch_pin(ENCODER_L_TIMER, CH_1, ENCODER_L_CHA_PORT, ENCODER_L_CHA_PIN,
					ENCODER_L_PIN_PULLUP);
	kb_timer_ch_pin(ENCODER_L_TIMER, CH_2, ENCODER_L_CHB_PORT, ENCODER_L_CHB_PIN,
					ENCODER_L_PIN_PULLUP);

	// init right channel
	kb_timer_ch_pin(ENCODER_R_TIMER, CH_1, ENCODER_R_CHA_PORT, ENCODER_R_CHA_PIN,
					ENCODER_R_PIN_PULLUP);
	kb_timer_ch_pin(ENCODER_R_TIMER, CH_2, ENCODER_R_CHB_PORT, ENCODER_R_CHB_PIN,
					ENCODER_R_PIN_PULLUP);


	// init timer
	// left channel
	kb_encoder_init_t enc_setting = {
			.direction = CCW,
			.prescaler = 0x0U
	};
	kb_encoder_init(ENCODER_L_TIMER, &enc_setting);
	
	// right channel
	enc_setting = (kb_encoder_init_t){
			.direction = CW,
			.prescaler = 0x0U
	};
	kb_encoder_init(ENCODER_R_TIMER, &enc_setting);


	// start encoders
	kb_encoder_start(ENCODER_L_TIMER);
	kb_encoder_start(ENCODER_R_TIMER);

	encoder_left_reset();
	encoder_right_reset();
}

int32_t encoder_left_count(void)
{
	return kb_encoder_count(ENCODER_L_TIMER);
}

int32_t encoder_right_count(void)
{
	return kb_encoder_count(ENCODER_R_TIMER);
}

void encoder_left_reset(void)
{
	kb_encoder_set(ENCODER_L_TIMER, 10000);
}

void encoder_right_reset(void)
{
	kb_encoder_set(ENCODER_R_TIMER, 10000);
}
