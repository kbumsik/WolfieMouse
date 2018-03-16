/*
 * pid_motor.h
 *
 *  Created on: Mar 20, 2017
 *      Author: kbumsik
 */

#ifndef PID_SPEED_H_
#define PID_SPEED_H_

#include "common_header.h"
#include "pid.h"

#ifdef __cplusplus
extern "C" {
#endif

status_t pid_speed_init(pid_value_t *pid_translation, pid_value_t *rotation);
status_t pid_speed_reset_error(void);
int32_t pid_speed_set_translation(int32_t translation);
int32_t pid_speed_set_rotation(int32_t rotation);

#ifdef __cplusplus
}
#endif

#endif /* PID_SPEED_H_ */
