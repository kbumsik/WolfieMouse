/*
 * motion_controller.c
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#include "system_config.h"
#include "common_source.h"
#include "range.h"
#include "pid.h"
#include "tick.h"
#include "cmd.h"

#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "control_loop_thread_driver.h"

/*******************************************************************************
 * Global variable
 ******************************************************************************/
struct range_data g_range;

/*******************************************************************************
 * Control loop thread
 ******************************************************************************/
static void control_loop(void *pvParameters);
static TaskHandle_t control_loop_handler;

static QueueHandle_t cmd_queue = NULL;
static SemaphoreHandle_t cmd_semphr = NULL;

/*******************************************************************************
 * Function definition
 ******************************************************************************/
void control_loop_thread_init(void)
{
    /* Allocate Queue */
    cmd_queue = xQueueCreate( 10, sizeof(struct cmd_queue_element));
    if (NULL == cmd_queue) {
        KB_DEBUG_ERROR("Creating cmd queue failed!!");
    }
    /* Allocate Semaphore */
    cmd_semphr = xSemaphoreCreateCounting(1, 0);

    /* Allocate task */
    BaseType_t result;
    result = xTaskCreate(
            control_loop,
            "Cnrt",
            configMINIMAL_STACK_SIZE+2000,
            NULL,
            configMAX_PRIORITIES - 1,
            &control_loop_handler);
    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating motion task failed!!");
    }
}


static void control_loop(void *pvParameters)
{
    static struct cmd_queue_element cmd;

    // PID handler
    static struct mouse_data_pid pid;

    range_get(&g_range, RANGE_CH_ALL); // To prevent the maze solver get wrong values

    while (1) {
        // Wait for a command from the maze solver
        xQueueReceive(cmd_queue, &cmd, portMAX_DELAY);
        // Invoke the corresponding control function
        control_loop_driver[cmd.type](&pid);
        // Notify the solver
        range_get(&g_range, RANGE_CH_ALL); // To prevent the maze solver get wrong values
        xSemaphoreGive(cmd_semphr);
    }

    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}

void control_loop_thread_wait_1ms(void)
{
    vTaskDelay(1);
}

QueueHandle_t control_loop_thread_cmd_queue(void)
{
    return cmd_queue;
}

SemaphoreHandle_t control_loop_thread_get_cmd_semphr(void)
{
    return cmd_semphr;
}
