/*
 * motion_controller.h
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#ifndef SRC_BOARD_THREAD_CONTROL_LOOP_H_
#define SRC_BOARD_THREAD_CONTROL_LOOP_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

void control_loop_thread_init(void);
void control_loop_thread_wait_1ms(void);
void control_loop_send_commend (struct cmd_command *cmd);
void control_loop_get_response (struct cmd_response *resp);
void control_loop_send_response (struct cmd_response *resp);

#ifdef __cplusplus
}
#endif

#endif /* SRC_BOARD_THREAD_CONTROL_LOOP_H_ */
