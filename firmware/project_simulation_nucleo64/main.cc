#include "trace.h"
#include "tick.h"
// KB library
#include "system_config.h"
#include "gpio.h"
#include "terminal.h"
#include "adc.h"
#include "flash.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

// Algorithm
#include "MouseController.hpp"
#include "IOInterface.hpp"
#include "FakeIO.hpp"
#include "SimulMouse.hpp"
#include "StdIO.hpp"
#include "FlashIO.hpp"

// Maze String file
#include "mazeString.hpp"

// Thread modules
#include "logger_thread.h"

void on_b1_pressed(void);

// Task functions
static void task_1_mouse_simulation();
static void task_2_logger_test();

// FreeRTOS threads
static void thread_main(void *pvParameters);
static void thread_blinky(void *pvParameters);

/* Task Handlers */
TaskHandle_t task_blinky_handler;
TaskHandle_t task_main_handler;
static SemaphoreHandle_t b1_semphr = NULL;
static SemaphoreHandle_t one_clikc_semphr = NULL;
static SemaphoreHandle_t double_click_semphr = NULL;

static void wait_for_button(MouseController *mouse);
/* peripheral objects */
// adc_t range_front_right;

/*******************************************************************************
 * Tasks
 ******************************************************************************/
static void task_1_mouse_simulation ()
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
                return;
            }
        }
    }
}

void task_2_logger_test ()
{
    for(int i = 0; i < 1000; i++) {
        logger_log(NULL, NULL, NULL, NULL, 100, 1200);
        vTaskDelay(1);
    }
}

/*******************************************************************************
 * Tasks
 ******************************************************************************/
static void thread_main(void *pvParameters)
{
    char buffer[80];
    char *keyinput = buffer;
    char key;
    puts("Please select a task");
    puts("1: Mouse simulation, ");
    puts("2: Logger test");
    fflush(stdout);
    while (true) {
        if (!terminal_gets(keyinput)) {
            // TODO: why it gets error? It seems to get error and sucess back and forth.
            // puts("Error!");
        }
        key = keyinput[0];
        switch (key) {
            case '1':
                task_1_mouse_simulation();
                break;

            case '2':
                task_2_logger_test();
                break;

            default:
                continue;
        }
        keyinput[0] = ' ';
    }
    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}

/* vBlinkyTask function */
void thread_blinky(void *pvParameters)
{
    portTickType xLastWakeTime;
    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */
    xLastWakeTime = xTaskGetTickCount();

    while (1) {
        // Wait for the first pressing.
        xSemaphoreTake(b1_semphr, portMAX_DELAY);
        gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_SET);

        // wait for the second until 500 ms
        if (pdTRUE == xSemaphoreTake(b1_semphr, (500 / portTICK_RATE_MS))) {
            xSemaphoreGive(double_click_semphr);
        } else {
            xSemaphoreGive(one_clikc_semphr);
        }
        gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);

        /* Call this Task explicitly every 10ms ,NOT Delay for 10ms */
        vTaskDelayUntil(&xLastWakeTime, (10 / portTICK_RATE_MS));
    }

    /* It never goes here, but the task should be deleted when it reaches here */
    vTaskDelete(NULL);
}

/*******************************************************************************
 * Other static functions
 ******************************************************************************/
static void wait_for_button(MouseController *mouse)
{
    char buffer[80];
    char *keyinput = buffer;
    char key;
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
 * Main function
 ******************************************************************************/
int main(void)
{
    // initialize clock and system configuration
    system_init();

    // Initialize all configured peripherals
    peripheral_init();

    // Init logger thread
    logger_thread_init();

    // ADC
    // This is A5 Pin in Nucleo-64
    // adc_init_t adc = {
    //         .device = RECV_ADC,
    //         .channel = KB_ADC_CH10
    // };
    // adc_init(&range_front_right, &adc);
    // adc_pin(RECV_FR_PORT, RECV_FR_PIN);

    // Set interrupt button
    gpio_init_t GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = NOPULL;
    gpio_isr_enable(B1_PORT, B1_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B1_PORT, B1_PIN, on_b1_pressed);
    b1_semphr = xSemaphoreCreateCounting(1, 0);
    one_clikc_semphr = xSemaphoreCreateCounting(1, 0);
    double_click_semphr = xSemaphoreCreateCounting(1, 0);

    // Set toggling pin controlled by the button
    // This pin controls the infared LED.
    // This is A0 pin in Nucleo-64
    // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    // GPIO_InitStruct.Pull = GPIO_NOPULL;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    // gpio_init(PORTA, PIN_0, &GPIO_InitStruct);
    // gpio_set(PORTA, PIN_0, GPIO_PIN_RESET);

    BaseType_t result;
    /* definition and creation of defaultTask */
    result = xTaskCreate(
            thread_blinky,            /* Pointer to the function that implements the task */
            "Blinky",               /* Text name for the task. This is to facilitate debugging only. It is not used in the scheduler */
            configMINIMAL_STACK_SIZE, /* Stack depth in words */
            NULL,                   /* Pointer to a task parameters */
            configMAX_PRIORITIES - 3,                      /* The task priority */
            &task_blinky_handler);  /* Pointer of its task handler, if you don't want to use, you can leave it NULL */

    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating blinky task failed!!");
    }

    result = xTaskCreate(
            thread_main,
            "Main",
            configMINIMAL_STACK_SIZE+15500,
            NULL,
            configMAX_PRIORITIES,
            &task_main_handler);
    if (result != pdPASS) {
        terminal_puts("Creating main task failed!!");
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
void on_b1_pressed(void)
{
    if (b1_semphr != NULL) {
        xSemaphoreGiveFromISR(b1_semphr, NULL);
    }
    return;
}
