
// FreeRTOS
#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "thread_solver_loop.hpp"
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


// Simple displya tasks
void task_disp_test1(void);
void task_disp_test2(void);
void task_disp_test3(void);
void task_disp_test4(void);

// Define message base. This is used for KB_DEBUG_MSG()
#ifdef KB_MSG_BASE
	#undef KB_MSG_BASE
	#define KB_MSG_BASE "Main"
#endif

/*******************************************************************************
 * Function declarations
 ******************************************************************************/

// Event declarations
static void on_b1_pressed(void);
static void on_b2_pressed(void);
static void on_task_done(void);

// Task declarations
static void task_main(void *pvParameters);
static void task_blinky(void *pvParameters);

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
TaskHandle_t task_main_handler;
TaskHandle_t task_blinky_handler;

#ifdef KB_WOLFIEMOUSE
TaskHandle_t task_range_handler;
#endif


/*******************************************************************************
 * State machine definitions
 ******************************************************************************/
// FSM action functions
static void disp_main(void);
static void disp_mem(void);
static void search_to_goal(void);
static void rush_to_goal(void);
static void task_print_mem(void);
static void task_reset_mem(void);
static void task_do_nothing(void);

// event enum
enum event {
    b1_pressed, b2_pressed, wheel_up, wheel_down, eol
};

// state enum
enum state {
    main_state, mem, test1, test2, test3, test4
};

// transition and table
struct transition {
    enum event event_val;
    void (*task) (void);
    enum state next_state;
};

// stopwatch current state
enum state current_state = main_state;

// fsm function
void main_fsm(enum event event_input){
    static const struct transition main_transitions[] = {
    //  Event        Task             Next_state
        {b1_pressed, search_to_goal,  main_state},
        {b2_pressed, search_to_goal,    main_state},
        {wheel_up,   disp_mem,        mem},
        {wheel_down, task_disp_test4,      test4},
        {eol,        disp_mem,      main_state}
    };
    static const struct transition mem_transitions[] = {
    //  Event        Task        Next_state
        {b1_pressed, task_print_mem,  mem},
        {b2_pressed, task_reset_mem,  mem},
        {wheel_up,   task_disp_test1, test1},
        {wheel_down, disp_main,  main_state},
        {eol,        disp_mem, mem}
    };
    static const struct transition test1_transitions[] = {
    //  Event        Task        Next_state
        // {b1_pressed, task_test1_time,  test1},
        // {b2_pressed, task_test1_time,  test1},
        {wheel_up,   task_disp_test2, test2},
        {wheel_down, disp_mem, mem},
        {eol,        task_disp_test1, test1}
    };
    static const struct transition test2_transitions[] = {
    //  Event        Task        Next_state
        // {b1_pressed, task_test2_CCW,  test2},
        // {b2_pressed, task_test2_CW,  test2},
        {wheel_up,   task_disp_test3,   test3},
        {wheel_down, task_disp_test1,   test1},
        {eol,        task_disp_test2, test2}
    };
    static const struct transition test3_transitions[] = {
    //  Event        Task        Next_state
        // {b1_pressed, task_test3_walls,  test3},
        // {b2_pressed, task_test3_walls,  test3},
        {wheel_up,   task_disp_test4,   test4},
        {wheel_down, task_disp_test2,   test2},
        {eol,        task_disp_test3, test3}
    };
    static const struct transition test4_transitions[] = {
    //  Event        Task        Next_state
        // {b1_pressed, task_test1_distance,  test4},
        // {b2_pressed, task_test1_distance,  test4},
        {wheel_up,   disp_main,   main_state},
        {wheel_down, task_disp_test3,   test3},
        {eol,        task_disp_test4, test4}
    };
    // FSM table. Since it is const, it will be stored in FLASH
    static const struct transition *fsm_table[6] = {
        main_transitions,
        mem_transitions,
        test1_transitions,
        test2_transitions,
        test3_transitions,
        test4_transitions,
    };

    // search for signal
    int i;
    for (i = 0; (fsm_table[current_state][i].event_val != event_input)
        && (fsm_table[current_state][i].event_val != eol); i++){
    };
    // call task function and than change state
    fsm_table[current_state][i].task();
    current_state = fsm_table[current_state][i].next_state;
    return;
}

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

    // Initialize all configured peripherals and then start control loop
    thread_control_loop_init();

    // Initialize command system
    cmd_init();

    cmd_low_pid_reset_and_stop(NULL);

    /* Task creation and definition */
    BaseType_t result;
//    result = xTaskCreate(
//            task_blinky,            /* Pointer to the function that implements the task */
//            "Blinky",               /* Text name for the task. This is to facilitate debugging only. It is not used in the scheduler */
//            configMINIMAL_STACK_SIZE+500, /* Stack depth in words */
//            NULL,                   /* Pointer to a task parameters */
//            configMAX_PRIORITIES,                      /* The task priority */
//            &task_blinky_handler);  /* Pointer of its task handler, if you don't want to use, you can leave it NULL */
//    if (result != pdPASS) {
//        KB_DEBUG_ERROR("Creating task failed!!");
//    }

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
    hcms_290x_matrix("HI  ");

    portTickType xLastWakeTime;
    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */
    xLastWakeTime = xTaskGetTickCount();

    uint32_t current_steps, last_steps;
    encoder_get(&counter, ENCODER_CH_RIGHT);
    last_steps = counter.right;
    while (1) {
        // polling button states
        if (is_b1_pressed) {
            main_fsm(b1_pressed);
            is_b1_pressed = 0;
            continue;
        } else if (is_b2_pressed) {
            main_fsm(b2_pressed);
            is_b2_pressed = 0;
            continue;
        }
        // polling left wheel state
        encoder_get(&counter, ENCODER_CH_RIGHT);
        current_steps = counter.right;
        if (current_steps > (last_steps + 300)) {
            last_steps = current_steps;
            main_fsm(wheel_up);
            continue;
        } else if (current_steps < (last_steps - 300)){
            last_steps = current_steps;
            main_fsm(wheel_down);
            continue;
        }
        /* keep running */
        //vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_RATE_MS));
    }
    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}


static void task_blinky(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    uint32_t seconds = 0;

    while (1) {
//         gpio_toggle(LED1_PORT, LED1_PIN);
//         gpio_toggle(LED2_PORT, LED2_PIN);
//         gpio_toggle(LED3_PORT, LED3_PIN);
//         gpio_toggle(LED4_PORT, LED4_PIN);
// #ifdef KB_BLACKWOLF
//         gpio_toggle(LED5_PORT, LED5_PIN);
//         gpio_toggle(LED6_PORT, LED6_PIN);
// #endif
        // terminal_puts("Blink!\n");

        //hcms_290x_int(seconds);
        ++seconds;        // Count seconds on the trace device.
        // trace_printf("Second %u\n", seconds);

        /* Call this Task every 1000ms */
        vTaskDelayUntil(&xLastWakeTime, (3000 / portTICK_RATE_MS));
    }
    vTaskDelete(NULL);
}


/*******************************************************************************
 * FSM action function
 ******************************************************************************/
static void disp_main(void)
{
    hcms_290x_matrix("Main");
}

static void disp_mem(void)
{
    hcms_290x_matrix("Mem");
}

static void search_to_goal(void)
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

    /* Motor test running done */
    main_fsm(eol);
}

static void rush_to_goal(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
    main_fsm(eol);
}

static void task_print_mem(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
    main_fsm(eol);
}

static void task_reset_mem(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
    main_fsm(eol);
}

static void task_do_nothing(void)
{
    return; // Do Nothing, literally.
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

static void on_task_done(void)
{
    // xSemaphoreGive(semphr_task);
}


void task_disp_test1(void)
{
    hcms_290x_matrix("TST1");
}

void task_disp_test2(void)
{
    hcms_290x_matrix("TST2");
}

void task_disp_test3(void)
{
    hcms_290x_matrix("TST3");
}

void task_disp_test4(void)
{
    hcms_290x_matrix("TST4");
}