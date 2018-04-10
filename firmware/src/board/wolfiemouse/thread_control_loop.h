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

void thread_control_loop_init(void);
QueueHandle_t thread_control_loop_request_queue(void);
QueueHandle_t thread_control_loop_response_queue(void);


#ifdef __cplusplus
}
#endif

#endif /* SRC_BOARD_THREAD_CONTROL_LOOP_H_ */
