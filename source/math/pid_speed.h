/*
 * pid_motor.h
 *
 *  Created on: Mar 20, 2017
 *      Author: kbumsik
 */

#ifndef PID_SPEED_H_
#define PID_SPEED_H_

#include "kb_common_header.h"
#include "pid.h"

#ifdef __cplusplus
extern "C" {
#endif

kb_status_t pid_speed_init(pid_t *pid_translation, pid_t *rotation);
kb_status_t pid_speed_reset_error(void);
int32_t pid_speed_set_translation(int32_t translation);
int32_t pid_speed_set_rotation(int32_t rotation);

#ifdef __cplusplus
}
#endif

#endif /* PID_SPEED_H_ */
