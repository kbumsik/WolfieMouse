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

#ifdef __cplusplus
extern "C" {
#endif

void control_loop_thread_init(void);
void control_loop_thread_wait_1ms(void);
QueueHandle_t control_loop_thread_cmd_queue(void);
SemaphoreHandle_t control_loop_thread_get_cmd_semphr(void);

extern struct range_data g_range;

#ifdef __cplusplus
}
#endif

#endif /* SRC_BOARD_THREAD_CONTROL_LOOP_H_ */
