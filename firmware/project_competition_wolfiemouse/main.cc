// In the same folder
#include "main_fsm_table.hpp"
#include "logger_thread.h"
#include "button.hpp"

// FreeRTOS
#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "config_measurements.h"

// KB library
#include "tick.h"
#include "trace.h"
#include "gpio.h"
#include "hcms_290x_display.h"
#include "terminal.h"
#include "encoder.h"
#include "system_config.h"
#include "control_loop_thread.h"
#include "cmd.h"
#include "range.h"

// Maze solver
#include "MouseController.hpp"
#include "IOInterface.hpp"
#include "RealMouse.hpp"
#include "StdIO.hpp"
#include "FlashIO.hpp"

// Define message base. This is used for KB_DEBUG_MSG()
#ifdef KB_MSG_BASE
#    undef KB_MSG_BASE
#    define KB_MSG_BASE "Main"
#endif

/*******************************************************************************
 * Function declarations and local variables
 ******************************************************************************/
// Maze solver
static void _maze_solver_run(void (*wait_func)(MouseController *mouse));
static void _wait_for_button(MouseController *mouse);

// Thread declarations
static void _thread_main(void *pvParameters);

/*******************************************************************************
 * FSM action function
 ******************************************************************************/
/**
 * @brief Competition mode. This task is used for actual competition.
 */
void task_1(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        gpio_toggle(LED2_PORT, LED2_PIN);
        delay_ms(500);
    }
    hcms_290x_matrix("PRSS");

    // wait for button pressed
    button_b1_b2_wait();

    hcms_290x_matrix("Go");
    delay_ms(1000);
    hcms_290x_matrix("    ");   // We should print empty spaces
                                // in order to save battery

    /* Go to main algorithm */
    _maze_solver_run(NULL);
}

/**
 * @brief Remote algorithm debugging mode. You can control the mouse
 *        step-by-step using serial console.
 */
void task_2(void)
{
    /* Not implemented */
    hcms_290x_matrix("Non2");
    delay_ms(2000);

    /* Give time to get ready */
    hcms_290x_matrix("PRSS");

    // wait for button pressed
    button_b1_b2_wait();

    hcms_290x_matrix("Go");
    delay_ms(1000);
    hcms_290x_matrix("    ");   // We should print empty spaces
                                // in order to save battery

    /* Go to main algorithm */
    _maze_solver_run(_wait_for_button);
}

/**
 * @brief Use front sensors to adjust micromouse.
 *        See https://www.youtube.com/watch?v=6PKUbhkdqWk
 */
void task_3(void)
{
    /* Not implemented */
    hcms_290x_matrix("Non3");
    delay_ms(2000);
}

/**
 * @brief Forward movement debugging mode. Mouse will move forward for 4 cells.
 */
void task_4(void)
{
    hcms_290x_matrix("Non4");
    delay_ms(2000);

    // Move
    cmd_polling(CMD_MOVE_FORWARD_ONE_CELL, NULL);
    cmd_polling(CMD_MOVE_FORWARD_ONE_CELL, NULL);
    cmd_polling(CMD_MOVE_FORWARD_ONE_CELL, NULL);
    cmd_polling(CMD_MOVE_FORWARD_ONE_CELL, NULL);
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
}

/**
 * @brief Pivot-left debugging mode. Mouse will pivot left.
 */
void task_5(void)
{
    hcms_290x_matrix("Non5");
    delay_ms(1000);

    // Pivot
    cmd_polling(CMD_PIVOT_LEFT_90_DEGREE, NULL);
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);

    // Pivot 180 degree
    delay_ms(1000);
    cmd_polling(CMD_PIVOT_LEFT_90_DEGREE, NULL);
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
    delay_ms(1000);
    cmd_polling(CMD_PIVOT_LEFT_90_DEGREE, NULL);
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
    delay_ms(1000);
}

/**
 * @brief Pivot-right debugging mode. Mouse will pivot left.
 */
void task_6(void)
{
    hcms_290x_matrix("Non6");
    delay_ms(2000);

    // Pivot
    cmd_polling(CMD_PIVOT_RIGHT_90_DEGREE, NULL);
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);

    // Pivot 180 degree
    delay_ms(1000);
    cmd_polling(CMD_PIVOT_RIGHT_90_DEGREE, NULL);
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
    delay_ms(1000);
    cmd_polling(CMD_PIVOT_RIGHT_90_DEGREE, NULL);
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
    delay_ms(1000);
}

/*******************************************************************************
 * Maze_solver
 ******************************************************************************/
static void _maze_solver_run(void (*wait_func)(MouseController *mouse))
{
    // Maze solver state
    enum {
        goGoal  = 0,
        goStart = 1,
        explore = 2
    } mouseState = goGoal;

    // Create virtual mouse hardware for simulation
    static FlashIO flashIO;
    static StdIO printIO(false);
    static RealMouse realmouse;
    static MouseController mouse(NULL, &flashIO, &printIO,
            (FinderInterface*) &realmouse, (MoverInterface*) &realmouse);

    /* First get ready and print maze */
    realmouse.getReady();
    mouse.printMaze();

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
            if (!mouse.scanAndMove(wait_func)) {
                goto end;
            }
        }
        taskYIELD();
    }

end:
    return;
}

static void _wait_for_button(MouseController *mouse)
{
    char buffer[80];
    char *keyinput = buffer;
    char key;

    // Stop the mouse completely and print the maze and the information.
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
    mouse->printMaze();
    puts("please input a command");
    puts("n:next, q: save and restart, p: print stack");
    fflush(stdout);

    while (true) {
        if (!terminal_gets(keyinput)) {
            // TODO: why it gets error? It seems to get error and sucess back and forth.
            // puts("Error!");
        }
        key = keyinput[0];
        keyinput[0] = ' ';
        if (key == 'n') {
            break;
        } else if (key == 'q') {
            goto save_and_exit;
        } else if (key == 'p') {
            mouse->printPathStack();
            fflush(stdout);
        } else {
            continue;
        }
        taskYIELD();
    }
    return;
save_and_exit:
    // Save it to flash
    mouse->saveMazeFile(NULL);
    puts("Maze saved to flash");
    puts("Good Bye!");
    exit(0);
    return;
}

/*******************************************************************************
 * Main thread
 ******************************************************************************/
static void _thread_main(void *pvParameters)
{
    static struct encoder_data counter;
    static uint32_t current_steps, last_steps;

    // This will display "TSK1" after showing "BOOT"
    hcms_290x_matrix("BOOT");
    delay_ms(2000);
    main_fsm_run_task(eol);

    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */
    // portTickType xLastWakeTime = xTaskGetTickCount();

    encoder_get(&counter, ENCODER_CH_RIGHT);
    last_steps = counter.right;
    while (1) {
        // polling button states
        if (button_b1_check()) {
            main_fsm_run_task(b1_pressed);
            continue;
        } else if (button_b2_check()) {
            main_fsm_run_task(b2_pressed);
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
    button_init();

    // Initialize all configured peripherals and then start control loop
    control_loop_thread_init();

    // Init logger thread
    logger_thread_init();

    // Initialize command system
    cmd_init();
    cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);

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
