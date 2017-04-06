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

typedef struct _pid_t {
    float kp;
    float ki;
    float kd;
} pid_value_t;

typedef struct _pid_handler_t {
    pid_value_t pid;
    int32_t setpoint;
    int32_t error;
    int32_t error_old;
    int32_t error_diff;
    int32_t error_sum;
} pid_handler_t;

void pid_reset(pid_handler_t *h);   // clear all errors
void pid_set_pid(pid_handler_t *h, pid_value_t *pid); // Set PID
void pid_input_setpoint(pid_handler_t *h, int32_t setpoint);
int32_t pid_compute(pid_handler_t *h, int32_t feedback);

#ifdef __cplusplus
}
#endif

#endif /* PID_H_ */
