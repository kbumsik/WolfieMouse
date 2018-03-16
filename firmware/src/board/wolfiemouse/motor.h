/*
 * motor.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Bumsik Kim
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "common_source.h"

typedef enum {
	CH_RIGHT = 0x0,
	CH_LEFT = 0x1,
	CH_BOTH = 0x4
} motor_ch_t;

#ifdef __cplusplus
extern "C" {
#endif

status_t motor_init(void);
void motor_dir_forward(void);
void motor_dir_backward(void);
void motor_dir_rotate_left(void);
void motor_dir_rotate_right(void);

int32_t motor_speed_percent(motor_ch_t channel, int32_t speed);
status_t motor_start(motor_ch_t eChannel);
status_t motor_stop(motor_ch_t eChannel);

#ifdef __cplusplus
}
#endif


#endif /* MOTOR_H_ */
