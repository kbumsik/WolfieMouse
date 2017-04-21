/*
 * motion_controller.c
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#include "motion_controller.h"
#include "system_control.h"
#include "kb_common_source.h"
#include "kb_encoder.h"
#include "encoder.h"
#include "config_measurements.h"
#include "pid.h"

#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*******************************************************************************
 * Global variables import
 ******************************************************************************/
// PID handler
extern pid_handler_t g_pid_T;
extern pid_handler_t g_pid_R;

/*******************************************************************************
 * Global variables
 ******************************************************************************/
uint32_t g_motion_target_L = 0;
uint32_t g_motion_target_R = 0;
SemaphoreHandle_t g_motion_semphr;

/*******************************************************************************
 * Private variables and functions
 ******************************************************************************/
static void process_cmd(motion_cmd_t *cmd);
static void task_motion(void *pvParameters);
void on_motion_completed (void);

static void (*completed_callback)(void);
static TaskHandle_t task_motion_handler;
static QueueHandle_t queue_motion;


/*******************************************************************************
 * Function definition
 ******************************************************************************/
void motion_controller_init()
{
    /* Allocate task */
    BaseType_t result;
    result = xTaskCreate(
            task_motion,
            "Mot",
            configMINIMAL_STACK_SIZE+2000,
            NULL,
            configMAX_PRIORITIES,
            &task_motion_handler);
    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating motion task failed!!");
    }

    /* Allocate Queue */
    queue_motion = xQueueCreate( 10, sizeof(motion_cmd_t));
    if (queue_motion == NULL) {
        KB_DEBUG_ERROR("Creating motion queue failed!!");
    }

    /* Allocate Semaphore */
    // Max counting is 2, initial is zero
    g_motion_semphr = xSemaphoreCreateCounting(2, 0);
}

void motion_queue(motion_cmd_t *commend)
{
    xQueueSend(queue_motion, commend, portMAX_DELAY);
}

static void process_cmd(motion_cmd_t *cmd)
{
    // reset the pid controllers
    //pid_reset(&g_pid_T);
    //pid_reset(&g_pid_R);

    switch (cmd->type) {
    case straight:
        system_enable_range_finder();
        system_reset_encoder();
        // set PID
        pid_input_setpoint(&g_pid_T, 18);
        pid_input_setpoint(&g_pid_R, 0);
        // Set input capture
        g_motion_target_L = MEASURE_ENCODER_DEFAULT + (MEASURE_STEPS_PER_CELL) * cmd->unit;
        system_motion_forward_L();
        g_motion_target_R = MEASURE_ENCODER_DEFAULT + (MEASURE_STEPS_PER_CELL) * cmd->unit;
        system_motion_forward_R();
        break;
    case turn:
        break;
    case curve:
        // Not implemented yet.
        break;
    default:
        break;
    }
    // Register on_completed callback function
    completed_callback = cmd->on_completed;

    // Invoke the on_start callback function.
    if (NULL != cmd->on_start) {
        cmd->on_start();
    }
}

static void task_motion(void *pvParameters)
{
    motion_cmd_t cmd;
    int cmd_available = 0;
    while (1) {
        xQueueReceive(queue_motion, &cmd, portMAX_DELAY);
        if (!cmd_available) {
            // If it is the first run after turn off motors,
            // power up motors
            cmd_available = 1;
            system_start_driving();
        }
        // Process the next commend
        process_cmd(&cmd);
        // wait for motion completed.
        // Take semaphore TWO times, left and right.
        xSemaphoreTake(g_motion_semphr, portMAX_DELAY);
        xSemaphoreTake(g_motion_semphr, portMAX_DELAY);

        // Invoke callback
        on_motion_completed();

        // Check if there is message wating
        cmd_available = uxQueueMessagesWaiting(queue_motion);
        KB_DEBUG_MSG("Motion available: %d\r\n", cmd_available);
        if (0 == cmd_available){
            //turn of motors
            system_stop_driving();
            system_disable_range_finder();
            pid_reset(&g_pid_T);
            pid_reset(&g_pid_R);
        }
    }
}

void on_motion_completed (void)
{
    // call callback
    if (completed_callback != NULL) {
        completed_callback();
    }
}
