/*
 * motion_controller.c
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#include "cmd.h"
#include "pid.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/*******************************************************************************
 * Private variables and functions
 ******************************************************************************/
static QueueHandle_t request_queue = NULL;
static QueueHandle_t response_queue = NULL;

/*******************************************************************************
 * Function definition
 ******************************************************************************/
void cmd_init(void)
{
    // /* Get queue from the controll loop */
    request_queue = thread_control_loop_request_queue();
    response_queue = thread_control_loop_response_queue();
    if ((request_queue == NULL) || (response_queue == NULL)) {
        KB_DEBUG_ERROR("Getting cmd queue failed!!");
    }
    return;
}

void cmd_low_pid_and_go(struct cmd_pid *cmd, struct cmd_events *events)
{
    // process events
    if (events != NULL) {
        if (events->on_start != NULL) {
            events->on_start();
        }
    }
    // int32_t step_left;  //< if it is zero, not target step
    // int32_t step_right; //< if it is zero, not target step
    struct cmd_request commend = {
        .type = CMD_LOW_PID_AND_GO,
        .pid = *cmd,
    };
    if (xQueueSend(request_queue, &commend, portMAX_DELAY) != pdPASS) {
        // failed to send a commend
    }
    // process events
    if (events != NULL) {
        if (events->on_completed != NULL) {
            events->on_completed();
        }
    }
}

void cmd_low_pid_reset_and_stop(struct cmd_events *events)
{
    // process events
    if (events != NULL) {
        if (events->on_start != NULL) {
            events->on_start();
        }
    }

    struct cmd_request commend = {
        .type = CMD_LOW_PID_RESET_AND_STOP,
    };
    if (xQueueSend(request_queue, &commend, portMAX_DELAY) != pdPASS) {
        // failed to send a commend
    }

    // process events
    if (events != NULL) {
        if (events->on_completed != NULL) {
            events->on_completed();
        }
    }
}

struct cmd_response cmd_polling(enum cmd_type type)
{
    struct cmd_request request = {
        .type = type,
    };
    if (xQueueSend(request_queue, &request, portMAX_DELAY) != pdPASS) {
        // failed to send a request
    }
    // Wait for semaphore

    struct cmd_response response;
    xQueueReceive(response_queue, &response, portMAX_DELAY);
    return response;
}
