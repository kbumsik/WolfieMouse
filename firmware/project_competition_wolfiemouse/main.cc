// In the same folder
#include "thread_solver_loop.hpp"
#include "main_fsm_table.hpp"

// FreeRTOS
#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "system_control.h"
#include "config_measurements.h"

// KB library
#include "tick.h"
#include "trace.h"
#include "gpio.h"
#include "hcms_290x_display.h"
#include "terminal.h"
#include "encoder.h"
#include "system_config.h"
#include "thread_control_loop.h"
#include "cmd.h"

// Define message base. This is used for KB_DEBUG_MSG()
#ifdef KB_MSG_BASE
#    undef KB_MSG_BASE
#    define KB_MSG_BASE "Main"
#endif

/*******************************************************************************
 * Function declarations
 ******************************************************************************/
// Event declarations
static void on_b1_pressed(void);
static void on_b2_pressed(void);

// Task declarations
static void thread_main(void *pvParameters);

/*******************************************************************************
 * local variables
 ******************************************************************************/
static int is_b1_pressed = 0;
static int is_b2_pressed = 0;
struct encoder_data counter;

/*******************************************************************************
 * Global variables
 ******************************************************************************/
// Task Handlers
TaskHandle_t thread_main_handler;

/*******************************************************************************
 * Main function
 ******************************************************************************/
int main(void)
{
    /* initialize clock and system configuration */
    system_init();

    /* Set Button Pressed Events */
    gpio_init_t GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;

    gpio_isr_enable(B1_PORT, B1_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B1_PORT, B1_PIN, on_b1_pressed);

    gpio_isr_enable(B2_PORT, B2_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B2_PORT, B2_PIN, on_b2_pressed);

    system_disable_range_finder();
    system_stop_driving();

    // Initialize all configured peripherals and then start control loop
    thread_control_loop_init();

    // Initialize command system
    cmd_init();

    system_disable_range_finder();
    system_stop_driving();
    cmd_low_pid_reset_and_stop(NULL);

    /* Task creation and definition */
    BaseType_t result;
    result = xTaskCreate(thread_main, "Main", configMINIMAL_STACK_SIZE + 15500, NULL,
                         configMAX_PRIORITIES - 2, &thread_main_handler);
    if (result != pdPASS) {
        terminal_puts("Creating task failed!!");
    }

    /* Do not put delay function in this section!
     * Because xTaskCreate will stop systick until the scheduler called */
    // TODO: check if it is true
    /* Start scheduler */
    vTaskStartScheduler();
    while (1) {
    }
}

/*******************************************************************************
 * Tasks
 ******************************************************************************/
static void thread_main(void *pvParameters)
{
    hcms_290x_matrix("BOOT");

    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */
    portTickType xLastWakeTime = xTaskGetTickCount();

    uint32_t current_steps, last_steps;
    encoder_get(&counter, ENCODER_CH_RIGHT);
    last_steps = counter.right;
    while (1) {
        // polling button states
        if (is_b1_pressed) {
            main_fsm_run_task(b1_pressed);
            is_b1_pressed = 0;
            continue;
        } else if (is_b2_pressed) {
            main_fsm_run_task(b2_pressed);
            is_b2_pressed = 0;
            continue;
        }
        // polling left wheel state
        encoder_get(&counter, ENCODER_CH_RIGHT);
        current_steps = counter.right;
        if (current_steps > (last_steps + 300)) {
            last_steps = current_steps;
            main_fsm_run_task(wheel_up);
            continue;
        } else if (current_steps < (last_steps - 300)) {
            last_steps = current_steps;
            main_fsm_run_task(wheel_down);
            continue;
        }

        /* Call this Task every 1000ms */
        // vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_RATE_MS));
    }
    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}

/*******************************************************************************
 * FSM action function
 ******************************************************************************/
void task_1(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        gpio_toggle(LED2_PORT, LED2_PIN);
        delay_ms(500);
    }
    // wait for button pressed
    while (!is_b2_pressed) {
    }
    is_b1_pressed = 0;

    hcms_290x_matrix("Go");
    delay_ms(1000);
    hcms_290x_matrix("    ");

    /* Go to main algorithm */
    main_run();
}

void task_2(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
}

void task_3(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
}

void task_4(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
}

void task_5(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
}

void task_6(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
}

/*******************************************************************************
 * Event handlers
 ******************************************************************************/
static void on_b1_pressed(void)
{
    is_b1_pressed = 1;
    trace_puts("B1 Pressed\n");
    return;
}

static void on_b2_pressed(void)
{
    is_b2_pressed = 1;
    trace_puts("B2 Pressed\n");
    return;
}
