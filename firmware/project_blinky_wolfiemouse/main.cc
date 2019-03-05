#include "trace.h"
#include "tick.h"
// KB library
#include "system_config.h"
#include "module_config.h"
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
#include "cmd.h"

// Algorithm
#include "MouseController.hpp"
#include "IOInterface.hpp"
#include "FakeIO.hpp"
#include "RealMouse.hpp"
#include "SimulMouse.hpp"
#include "StdIO.hpp"
#include "FlashIO.hpp"

// Maze String file
#include "mazeString.hpp"

static void on_b1_pressed(void);
static void on_b2_pressed(void);

static void task_blinky(void *pvParameters);
static void task_main(void *pvParameters);

/*******************************************************************************
 * local variables
 ******************************************************************************/
/* Task Handlers */
TaskHandle_t task_blinky_handler;
TaskHandle_t task_main_handler;
static SemaphoreHandle_t b1_semphr = NULL;
static SemaphoreHandle_t b2_semphr = NULL;

static void wait_for_button(MouseController *mouse);

/*******************************************************************************
 * Main function
 ******************************************************************************/
int main(void)
{
    // initialize clock and system configuration
    system_init();

    // Initialize all configured peripherals and then start control loop
    thread_control_loop_init();

    // Initialize command system
    cmd_init();

    // Set interrupt button
    gpio_init_t GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;

    gpio_isr_enable(B1_PORT, B1_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B1_PORT, B1_PIN, on_b1_pressed);
    b1_semphr = xSemaphoreCreateCounting(1, 0);

    gpio_isr_enable(B2_PORT, B2_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B2_PORT, B2_PIN, on_b2_pressed);
    b2_semphr = xSemaphoreCreateCounting(1, 0);

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

    result = xTaskCreate(
            task_main,
            "Main",
            configMINIMAL_STACK_SIZE+15500,
            NULL,
            configMAX_PRIORITIES-2,
            &task_main_handler);
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
static void task_main(void *pvParameters)
{
    // Create virtual mouse hardware for simulation
    static StdIO fileIO(true);
    static StdIO printIO(false);
    static FakeIO fakeIO;
    static FlashIO flashIO;

    // mazeString is located in mazeString.hpp
    static SimulMouse virtualMouse(const_cast<Maze::StringMaze *>(&mazeString), &fakeIO, &fakeIO);

    enum WolfieState {
        goGoal  = 0,
        goStart = 1,
        explore = 2
    } mouseState = goGoal;

    MouseController mouse(NULL, &flashIO, &printIO,
            (FinderInterface*) &virtualMouse, (MoverInterface*) &virtualMouse);

    hcms_290x_matrix("Sim.");
    /* First just print maze and wait */
    wait_for_button(&mouse);

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
            if (!mouse.scanAndMove(wait_for_button)) {
                goto end;
            }
        }
    }

end:
    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}

void task_blinky(void *pvParameters)
{
    portTickType xLastWakeTime;
    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */

    uint32_t seconds = 0;

    // terminal_puts("Hello World!\n");

    // hcms_290x_matrix("MAIN");

    gpio_toggle(LED1_PORT, LED1_PIN);
    gpio_toggle(LED2_PORT, LED2_PIN);
    gpio_toggle(LED3_PORT, LED3_PIN);
    gpio_toggle(LED4_PORT, LED4_PIN);
    gpio_toggle(LED5_PORT, LED5_PIN);
    gpio_toggle(LED6_PORT, LED6_PIN);

    // // Apply to the motor
    // struct cmd_pid setpoints = {
    //     .setpoint_trans = 20,
    //     .setpoint_rot = 0,
    //     // .step_left = 0,
    //     // .step_right = 0
    // };
    // cmd_low_pid_and_go(&setpoints, NULL);
    
    //  delay_ms(1000);

    // cmd_polling(CMD_MOVE_FORWARD_ONE_CELL);
    // cmd_polling(CMD_MOVE_FORWARD_ONE_CELL);
    // cmd_polling(CMD_MOVE_FORWARD_ONE_CELL);
    //cmd_polling(CMD_MOVE_FORWARD_ONE_CELL);
    // cmd_low_pid_reset_and_stop(NULL);
    /* Motor test running done */

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

/*******************************************************************************
 * static functions
 ******************************************************************************/
static void wait_for_button(MouseController *mouse)
{
    /* And print */
    mouse->printMaze();
    printf("please press a button\n");
    fflush(stdout);
    
    // Wait for the button pressed.
    xSemaphoreTake(b1_semphr, portMAX_DELAY);

    // Check if B2 pressed
    if (xSemaphoreTake(b2_semphr, 0) == pdTRUE) {
        // Then save it to FLASH
        mouse->saveMazeFile(NULL);
    }
}

/*******************************************************************************
 * Event handlers
 ******************************************************************************/
void on_b1_pressed(void)
{
    if (b1_semphr != NULL) {
        xSemaphoreGiveFromISR(b1_semphr, NULL);
        trace_puts("Button Pressed\n");
    }
    return;
}

static void on_b2_pressed(void)
{
    if (b2_semphr != NULL) {
        xSemaphoreGiveFromISR(b2_semphr, NULL);
        trace_puts("Button Pressed\n");
    }
    return;
}