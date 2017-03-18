/*
 * encoder.c
 *
 *  Created on: Nov 6, 2016
 *      Author: Bumsik Kim
 */

#include "encoder.h"
#include "kb_common_source.h"
#include "kb_timer.h"

void encoder_init(void)
{
	// init right channel
	//PB7	TIM4_CH2	Encoder_R_CHA
	//PB6	TIM4_CH1	Encoder_R_CHB
	kb_timer_ch_pin(TIMER4, CH_2, PORTB, PIN_7, PULLUP);
	kb_timer_ch_pin(TIMER4, CH_1, PORTB, PIN_6, PULLUP);

	// init left channel
	//PC7	TIM3_CH2	Encoder_L_CHA
	//PC6	TIM3_CH1	Encoder_L_CHB
	kb_timer_ch_pin(TIMER3, CH_2, PORTC, PIN_7, PULLUP);
	kb_timer_ch_pin(TIMER3, CH_1, PORTC, PIN_6, PULLUP);

	// init timer
	// right channel
	kb_encoder_init_t enc_setting = {
			.direction = CCW,
			.prescaler = 0xfU
	};
	kb_encoder_init(TIMER4, &enc_setting);

	// left channel
	enc_setting = (kb_encoder_init_t){
			.direction = CW,
			.prescaler = 0xfU
	};
	kb_encoder_init(TIMER3, &enc_setting);

	// start encoders
	kb_encoder_start(TIMER4);
	kb_encoder_start(TIMER3);

	encoder_right_reset();
	encoder_left_reset();
}


int32_t encoder_right_count(void)
{
	return kb_encoder_count(TIMER4);
}


int32_t encoder_left_count(void)
{
	return kb_encoder_count(TIMER3);
}


void encoder_right_reset(void)
{
	kb_encoder_set(TIMER4, 10000);
}

void encoder_left_reset(void)
{
	kb_encoder_set(TIMER3, 10000);
}
