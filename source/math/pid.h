/*
 * pid.h
 *
 *  Created on: Mar 20, 2017
 *      Author: kbumsik
 */

#ifndef PID_H_
#define PID_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float kp;
    float ki;
    float kd;
} pid_t;

typedef struct {
    pid_t pid;
    int32_t setpoint;
    float error;
    float error_old;
    float error_diff;
    float error_sum;
} pid_handler_t;

void pid_reset(pid_handler_t *h);
void pid_set_pid(pid_handler_t *h, pid_t *pid);
void pid_input_setpoint(pid_handler_t *h, int32_t setpoint);
int32_t pid_compute(pid_handler_t *h, int32_t feedback);

#ifdef __cplusplus
}
#endif

#endif /* PID_H_ */
