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
#include "control_loop_thread.h"

/*******************************************************************************
 * Function definition
 ******************************************************************************/
void cmd_init(void)
{
    return;
}

void cmd_polling(enum cmd_type type, int (*escape_callback)(struct cmd_response))
{
    struct cmd_command commend = {
        .type = type,
    };
    control_loop_send_commend(&commend);

    // Wait for response
    struct cmd_response response;
    while (1) {
        control_loop_get_response(&response);
        if (escape_callback) {
            if (escape_callback(response)) {
                break;
            }
        } else {
            if (CMD_RESP_DONE == response.type) {
                break;
            }
        }
    }
}
