// In the same folder
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

// Maze solver
#include "MouseController.hpp"
#include "IOInterface.hpp"
#include "FakeIO.hpp"
#include "RealMouse.hpp"

// Define message base. This is used for KB_DEBUG_MSG()
#ifdef KB_MSG_BASE
#    undef KB_MSG_BASE
#    define KB_MSG_BASE "Main"
#endif

/*******************************************************************************
 * Function declarations and local variables
 ******************************************************************************/
// Maze solver
void _maze_solver_run(void);

// Thread declarations
static void _thread_main(void *pvParameters);

// Event declarations
static void _on_b1_pressed(void);
static void _on_b2_pressed(void);

// Local variables
static int _is_b1_pressed = 0;
static int _is_b2_pressed = 0;

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
    while (!_is_b2_pressed) {
    }
    _is_b1_pressed = 0;

    hcms_290x_matrix("Go");
    delay_ms(1000);
    hcms_290x_matrix("    ");   // We should print empty spaces
                                // in order to save battery

    /* Go to main algorithm */
    _maze_solver_run();
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
 * Maze_solver
 ******************************************************************************/
void _maze_solver_run(void)
{
    char tmp;
    // Maze solver state
    enum {
        goGoal  = 0,
        goStart = 1,
        explore = 2
    } mouseState = goGoal;

    // Create virtual mouse hardware for simulation
    static FakeIO fake;
    static RealMouse realmouse;
    static MouseController mouse(NULL, &fake, &fake,
            (FinderInterface*) &realmouse, (MoverInterface*) &realmouse);

    /* First just print maze */
    //mouse.printMaze();

    while (true) {
        //Finite State Machine
        if (mouse.allDestinationsReached()) {
            switch (mouseState) {
            case goGoal:
                mouseState = explore;
                mouse.makeRandomDest(4);
                break;
            case explore:
                mouseState = goStart;
                mouse.makeStartAsDest();
                break;
            case goStart:
                mouseState = goGoal;
                mouse.makeGoalAsDest();
                break;
            default:
                //printf("Invalid state");
                break;
            }
        } else {
            if (!mouse.scanAndMove(NULL)) {
                goto end;
            }
        }
    }

end:
    return;
}

/*******************************************************************************
 * Main thread
 ******************************************************************************/
static void _thread_main(void *pvParameters)
{
    static struct encoder_data counter;
    static uint32_t current_steps, last_steps;

    hcms_290x_matrix("BOOT");

    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */
    // portTickType xLastWakeTime = xTaskGetTickCount();

    encoder_get(&counter, ENCODER_CH_RIGHT);
    last_steps = counter.right;
    while (1) {
        // polling button states
        if (_is_b1_pressed) {
            main_fsm_run_task(b1_pressed);
            _is_b1_pressed = 0;
            continue;
        } else if (_is_b2_pressed) {
            main_fsm_run_task(b2_pressed);
            _is_b2_pressed = 0;
            continue;
        }

        // Polling right wheel state
        // Rotation of wheel will be used to select a task.
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
 * Main function
 ******************************************************************************/
int main(void)
{
    static TaskHandle_t _thread_main_handler;

    /* initialize clock and system configuration */
    system_init();

    /* Set Button Pressed Events */
    gpio_init_t GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;

    gpio_isr_enable(B1_PORT, B1_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B1_PORT, B1_PIN, _on_b1_pressed);

    gpio_isr_enable(B2_PORT, B2_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B2_PORT, B2_PIN, _on_b2_pressed);

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
    result = xTaskCreate(
        _thread_main,               /* Pointer to the function that implements the task */
        "Main",                     /* Text name for the task. This is to facilitate debugging only. It is not used in the scheduler */
        configMINIMAL_STACK_SIZE + 15500, /* Stack depth in words */
        NULL,                       /* Pointer to a task parameters */
        configMAX_PRIORITIES - 2,   /* The task priority */
        &_thread_main_handler);     /* Pointer of its task handler, if you don't want to use, you can leave it NULL */
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
 * Event handlers
 ******************************************************************************/
static void _on_b1_pressed(void)
{
    _is_b1_pressed = 1;
    trace_puts("B1 Pressed\n");
    return;
}

static void _on_b2_pressed(void)
{
    _is_b2_pressed = 1;
    trace_puts("B2 Pressed\n");
    return;
}
