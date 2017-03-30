/*
 * pid.c
 *
 *  Created on: Mar 20, 2017
 *      Author: kbumsik
 */

#include "pid.h"

void pid_reset(pid_handler_t *h)
{
    h->setpoint = 0;
    h->error = 0;
    h->error_old = 0;
    h->error_diff = 0;
    h->sum = 0;
}

void pid_set_pid(pid_handler_t *h, pid_t *pid)
{
    h->pid.kp = pid->kp;
    h->pid.ki = pid->ki;
    h->pid.kd = pid->kd;
}

void pid_input_setpoint(pid_handler_t *h, int32_t setpoint)
{
    h->setpoint = setpoint;
}

int32_t pid_compute(pid_handler_t *h, int32_t feedback)
{
    h->error = h->setpoint - feedback;
    h->error_diff = h->error - h->error_old;
    h->error_sum += h->error;
    return (h->pid.kp * h->error) +
            (h->pid.ki * h->sum) +
            (h->pid.kd * h->error_diff);
}

