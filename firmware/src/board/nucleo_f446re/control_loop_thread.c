#include "control_loop_thread_driver.h"
#include "cmd.h"

#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

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
    /* Allocate Semaphore */
    // Max counting is 1, initial is zero
    semphr_from_isr = xSemaphoreCreateCounting(1, 0);
    /* Allocate Queue */
    command_queue = xQueueCreate( 2, sizeof(struct cmd_command));
    if (NULL == command_queue) {
        terminal_puts("Creating cmd command queue failed!!");
    }
    response_queue = xQueueCreate( 3, sizeof(struct cmd_response));
    if (NULL == response_queue) {
        terminal_puts("Creating cmd resp queue failed!!");
    }

    /* Allocate task */
    BaseType_t result;
    result = xTaskCreate(
            control_loop,
            "Cnrt",
            configMINIMAL_STACK_SIZE+2000,
            NULL,
            configMAX_PRIORITIES,
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

void control_loop_thread_wait_until_1ms(void)
{
    xSemaphoreTake(semphr_from_isr, portMAX_DELAY);
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

void SysTick_hook(void)
{
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (semphr_from_isr != NULL) {
        xSemaphoreGiveFromISR(semphr_from_isr, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
