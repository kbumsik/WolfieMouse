/*
 * system_control.h
 *
 *  Created on: Apr 7, 2017
 *      Author: kbumsik
 */

#ifndef SYSTEM_CONTROL_H_
#define SYSTEM_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

// Motor driving
void system_start_driving(void);
void system_stop_driving(void);

// Range finder control
void system_enable_range_finder(void);
void system_disable_range_finder(void);

// Motion controller flags
void system_motion_forward_L(void);
void system_motion_backward_L(void);

void system_motion_forward_R(void);
void system_motion_backward_R(void);

#ifdef __cplusplus
}
#endif


#endif /* SYSTEM_CONTROL_H_ */
