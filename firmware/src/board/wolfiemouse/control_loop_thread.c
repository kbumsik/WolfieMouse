/*
 * motion_controller.c
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#include "system_config.h"
#include "common_source.h"
#include "encoder.h"
#include "gpio.h"
#include "motor.h"
#include "range.h"
#include "terminal.h"
#include "hcms_290x_display.h"

#include "pid.h"
#include "tick.h"
#include "cmd.h"
#include "control_loop_thread.h"

#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "control_loop_thread_driver.h"

/*******************************************************************************
 * Control loop thread
 ******************************************************************************/
static void control_loop(void *pvParameters);
static TaskHandle_t control_loop_handler;

static SemaphoreHandle_t semphr_from_isr = NULL;
static QueueHandle_t command_queue = NULL;
static QueueHandle_t response_queue = NULL;

/*******************************************************************************
 * Function definition
 ******************************************************************************/
void control_loop_thread_init(void)
{
    gpio_init_t GPIO_InitStruct;

    /* Configure UART */
    terminal_init();

    /* Init motor */
    motor_init();
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);

    /* Configure Pushbuttons */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init(B1_PORT, B1_PIN, &GPIO_InitStruct);
    gpio_init(B2_PORT, B2_PIN, &GPIO_InitStruct);

    /* Configure LED pins */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init(LED1_PORT, LED1_PIN, &GPIO_InitStruct);
    gpio_init(LED2_PORT, LED2_PIN, &GPIO_InitStruct);
    gpio_init(LED3_PORT, LED3_PIN, &GPIO_InitStruct);
    gpio_init(LED4_PORT, LED4_PIN, &GPIO_InitStruct);
    gpio_init(LED5_PORT, LED5_PIN, &GPIO_InitStruct);
    gpio_init(LED6_PORT, LED6_PIN, &GPIO_InitStruct);

    /* Configure LED Pins Output Level */
    gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    gpio_set(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_RESET);
    gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_RESET);
    gpio_set(LED5_PORT, LED5_PIN, GPIO_PIN_RESET);
    gpio_set(LED6_PORT, LED6_PIN, GPIO_PIN_RESET);

    /* Init peripherals for the LED display */
    hcms_290x_init();

    /* Range Sensor */
    range_init();

    /* Init encoder */
    encoder_init();

    /* Now peripherals has been initialized */

    /* Allocate Queue */
    command_queue = xQueueCreate( 5, sizeof(struct cmd_command));
    if (NULL == command_queue) {
        KB_DEBUG_ERROR("Creating cmd queue failed!!");
    }
    response_queue = xQueueCreate( 10, sizeof(struct cmd_response));
    if (NULL == response_queue) {
        KB_DEBUG_ERROR("Creating cmd queue failed!!");
    }

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
    static struct cmd_command cmd;

    // PID handler
    static struct mouse_data_pid pid;

    while (1) {
        // Wait for a command from the maze solver
        xQueueReceive(command_queue, &cmd, portMAX_DELAY);
        // Invoke the corresponding control function
        control_loop_driver[cmd.type](&pid);
        // Notify the solver
        struct cmd_response resp = {
            .type = CMD_RESP_DONE,
            .range = (const struct range_data){0},
        };
        xQueueSend(response_queue, &resp, portMAX_DELAY);
    }

    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}

void control_loop_thread_wait_1ms(void)
{
    vTaskDelay(1);
}

void control_loop_send_commend (struct cmd_command *cmd)
{
    xQueueSend(command_queue, cmd, portMAX_DELAY);
}

void control_loop_get_response (struct cmd_response *resp)
{
    xQueueReceive(response_queue, resp, portMAX_DELAY);
}

void control_loop_send_response (struct cmd_response *resp)
{
    xQueueSend(response_queue, resp, portMAX_DELAY);
}
