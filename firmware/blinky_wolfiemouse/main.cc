#include "trace.h"
#include "tick.h"
// KB library
#include "system_config.h"
#include "gpio.h"
#include "terminal.h"
#include "hcms_290x_display.h"
#include "adc.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

// Micromouse system
#include "system_control.h"
#include "thread_control_loop.h"
#include "pid.h"
#include "encoder.h"
#include "range.h"
#include "motor.h"

void on_pressed(void);

static void task_blinky(void *pvParameters);

/* Task Handlers */
TaskHandle_t task_blinky_handler;

// PID handler
extern pid_handler_t g_pid_T;
extern pid_handler_t g_pid_R;

int main(void)
{
    // initialize clock and system configuration
    system_init();

    // Set interrupt button
    gpio_init_t GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = NOPULL;
    gpio_isr_enable(B1_PORT, B1_PIN, &GPIO_InitStruct, RISING_EDGE);
    gpio_isr_register(B1_PORT, B1_PIN, on_pressed);

    // Initialize all configured peripherals and then start control loop
    thread_control_loop_init();

    BaseType_t result;
    /* definition and creation of defaultTask */
    result = xTaskCreate(
            task_blinky,            /* Pointer to the function that implements the task */
            "Blinky",               /* Text name for the task. This is to facilitate debugging only. It is not used in the scheduler */
            configMINIMAL_STACK_SIZE, /* Stack depth in words */
            NULL,                   /* Pointer to a task parameters */
            0,   /* The task priority */
            &task_blinky_handler);  /* Pointer of its task handler, if you don't want to use, you can leave it NULL */

    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating task failed!!");
    }

    /* Do not put delay function in this section!
     * Because xTaskCreate will stop systick until the scheduler called */
    // TODO: check if it is true
    /* Start scheduler */
    vTaskStartScheduler();
    while (1) {
    }
}

void on_pressed(void)
{
    trace_puts("Button Pressed\n");
    return;
}

/* vBlinkyTask function */
void task_blinky(void *pvParameters)
{
    portTickType xLastWakeTime;
    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */

    uint32_t seconds = 0;

    // terminal_puts("Hello World!\n");

    hcms_290x_matrix("MAIN");

    gpio_toggle(LED1_PORT, LED1_PIN);
    gpio_toggle(LED2_PORT, LED2_PIN);
    gpio_toggle(LED3_PORT, LED3_PIN);
    gpio_toggle(LED4_PORT, LED4_PIN);
    gpio_toggle(LED5_PORT, LED5_PIN);
    gpio_toggle(LED6_PORT, LED6_PIN);

    // Apply to the motor
    pid_reset(&g_pid_T);
    pid_reset(&g_pid_R);

    system_enable_range_finder();
    pid_input_setpoint(&g_pid_T, 60);
    pid_input_setpoint(&g_pid_R, 0);
    system_start_driving();
    delay_ms(2000);
    /* Motor test running done */
    system_stop_driving();
    system_disable_range_finder();

    struct encoder_data step;
    struct range_data range;

    xLastWakeTime = xTaskGetTickCount();
    
    while (1) {


        gpio_toggle(LED1_PORT, LED1_PIN);
        gpio_toggle(LED2_PORT, LED2_PIN);
        gpio_toggle(LED3_PORT, LED3_PIN);
        gpio_toggle(LED4_PORT, LED4_PIN);
        gpio_toggle(LED5_PORT, LED5_PIN);
        gpio_toggle(LED6_PORT, LED6_PIN);
        ++seconds;

        /* Call this Task explicitly every 500ms ,NOT Delay for 50ms */
        vTaskDelayUntil(&xLastWakeTime, (500 / portTICK_RATE_MS));
    }

    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}
