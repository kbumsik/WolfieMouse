#include "system_control.h"
#include "main_tasks.hpp"
#include "config_measurements.h"

// KB library
#include "kb_tick.h"
#include "kb_trace.h"
#include "kb_gpio.h"
#include "kb_HCMS-290X_display.h"
#include "kb_terminal.h"
#include "kb_timer.h"
#include "kb_TCA9545A_i2c_mux.h"
#include "kb_VL6180X_range_finder.h"
#include "kb_adc.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

// User config
#include "system_config.h"
#include "encoder.h"
#include "motor.h"
#include "motion_controller.h"

// Algorithm
#include "pid.h"

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
#ifdef KB_WOLFIEMOUSE
static void task_range(void *pvParameters);
#endif

/*******************************************************************************
 * local variables
 ******************************************************************************/
static int is_b1_pressed = 0;
static int is_b2_pressed = 0;
SemaphoreHandle_t semphr_task;

/*******************************************************************************
 * Global variables
 ******************************************************************************/
// Task Handlers
TaskHandle_t task_main_handler;
TaskHandle_t task_blinky_handler;

#ifdef KB_WOLFIEMOUSE
TaskHandle_t task_range_handler;
#endif

// Mutex Handlers
SemaphoreHandle_t mutex_range;

/*
 * These global varables are located in peripherals.c
 */
// range sensor value
extern volatile uint16_t range_L, range_R, range_F, range_FL, range_FR;
// Encoder value
extern volatile int32_t steps_L, steps_R, steps_L_old, steps_R_old;
extern volatile int32_t speed_L, speed_R, speed_D;

// PID handler
extern pid_handler_t g_pid_T;
extern pid_handler_t g_pid_R;

/*******************************************************************************
 * State machine definitions
 ******************************************************************************/
// FSM action functions
static void disp_main(void);
static void disp_mem(void);
static void disp_test1(void);
static void disp_test2(void);
static void disp_test3(void);
static void disp_test4(void);
static void search_to_goal(void);
static void rush_to_goal(void);
static void print_mem(void);
static void reset_mem(void);
// Going straight test
static void run_test1_time(void);
static void run_test1_distance(void);
// Rotation test
static void run_test2_CW(void);
static void run_test2_CCW(void);
// Range Sensor test
static void run_test3_walls(void);
static void run_test4(void);
static void do_nothing(void);

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
        {wheel_down, disp_test4,      test4},
        {eol,        disp_mem,      main_state}
    };
    static const struct transition mem_transitions[] = {
    //  Event        Task        Next_state
        {b1_pressed, print_mem,  mem},
        {b2_pressed, reset_mem,  mem},
        {wheel_up,   disp_test1, test1},
        {wheel_down, disp_main,  main_state},
        {eol,        disp_mem, mem}
    };
    static const struct transition test1_transitions[] = {
    //  Event        Task        Next_state
        {b1_pressed, run_test1_time,  test1},
        {b2_pressed, run_test1_time,  test1},
        {wheel_up,   disp_test2, test2},
        {wheel_down, disp_mem, mem},
        {eol,        disp_test1, test1}
    };
    static const struct transition test2_transitions[] = {
    //  Event        Task        Next_state
        {b1_pressed, run_test2_CCW,  test2},
        {b2_pressed, run_test2_CW,  test2},
        {wheel_up,   disp_test3,   test3},
        {wheel_down, disp_test1,   test1},
        {eol,        disp_test2, test2}
    };
    static const struct transition test3_transitions[] = {
    //  Event        Task        Next_state
        {b1_pressed, run_test3_walls,  test3},
        {b2_pressed, run_test3_walls,  test3},
        {wheel_up,   disp_test4,   test4},
        {wheel_down, disp_test2,   test2},
        {eol,        disp_test3, test3}
    };
    static const struct transition test4_transitions[] = {
    //  Event        Task        Next_state
        {b1_pressed, run_test1_distance,  test4},
        {b2_pressed, run_test1_distance,  test4},
        {wheel_up,   disp_main,   main_state},
        {wheel_down, disp_test3,   test3},
        {eol,        disp_test4, test4}
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

    /* Initialize all configured peripherals */
    peripheral_init();
    system_stop_driving();
    system_disable_range_finder();

    /* Initial LED Display message */
    hcms_290x_matrix("BOOT");

    /* Wait for 3 sec */
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED1_PORT, LED1_PIN);
        kb_delay_ms(500);
    }

    /* Set Button Pressed Events */
    kb_gpio_init_t GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;

    kb_gpio_isr_enable(B1_PORT, B1_PIN, &GPIO_InitStruct, FALLING_EDGE);
    kb_gpio_isr_register(B1_PORT, B1_PIN, on_b1_pressed);

    kb_gpio_isr_enable(B2_PORT, B2_PIN, &GPIO_InitStruct, FALLING_EDGE);
    kb_gpio_isr_register(B2_PORT, B2_PIN, on_b2_pressed);

    /* Test message */
    trace_puts("Hello ARM World!");
    kb_terminal_puts("Hello World!\n");

    /* Mutex creation */
    mutex_range = xSemaphoreCreateMutex();

    /* Semaphore creation */
    semphr_task = xSemaphoreCreateBinary();

    /* first fsm action */
    disp_main();

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

#ifdef KB_WOLFIEMOUSE
    result = xTaskCreate(
            task_range,
            "Finder",
            configMINIMAL_STACK_SIZE+500,
            NULL,
            configMAX_PRIORITIES-1,
            &task_range_handler);
    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating task failed!!");
    }
#endif

    result = xTaskCreate(
            task_main,
            "Main",
            configMINIMAL_STACK_SIZE+10500,
            NULL,
            configMAX_PRIORITIES-2,
            &task_main_handler);
    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating task failed!!");
    }

    /* Do not put delay function in this section!
     * Because xTaskCreate will stop systick until the scheduler called */
    // TODO: check if it is true
    /* Start scheduler */
    motion_controller_init();
    vTaskStartScheduler();
    while (1) {
    }
}

/*******************************************************************************
 * Tasks
 ******************************************************************************/
static void task_main(void *pvParameters)
{
    portTickType xLastWakeTime;
    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */
    xLastWakeTime = xTaskGetTickCount();

    uint32_t current_steps;
    uint32_t last_steps = encoder_right_count();
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
        current_steps = encoder_right_count();
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

#ifdef KB_WOLFIEMOUSE
static void task_range(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while (1) {
        xSemaphoreTake(mutex_range, portMAX_DELAY); /* Take Mutext */
        //range_left = range_finder_get(left_side);
        //range_right = range_finder_get(right_side);
        //range_front_left = range_finder_get(left_front);
        //range_front_right = range_finder_get(right_front);
        //range_front = (range_front_left + range_front_right) / 2
        xSemaphoreGive(mutex_range);
        /* Call this Task every 50ms */
        vTaskDelayUntil(&xLastWakeTime, (50 / portTICK_RATE_MS));
    }
}
#endif

static void task_blinky(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    uint32_t seconds = 0;

    while (1) {
        kb_gpio_toggle(LED1_PORT, LED1_PIN);
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_gpio_toggle(LED3_PORT, LED3_PIN);
        kb_gpio_toggle(LED4_PORT, LED4_PIN);
#ifdef KB_BLACKWOLF
        kb_gpio_toggle(LED5_PORT, LED5_PIN);
        kb_gpio_toggle(LED6_PORT, LED6_PIN);
#endif
        kb_terminal_puts("Blink!\n");

        //hcms_290x_int(seconds);
        ++seconds;        // Count seconds on the trace device.
        trace_printf("Second %u\n", seconds);

        // Print speed
        KB_DEBUG_MSG("left encoder: %d\n", steps_L);
        KB_DEBUG_MSG("right encoder: %d\n", steps_R);
        KB_DEBUG_MSG("left speed: %d\n", speed_L);
        KB_DEBUG_MSG("right speed: %d\n", speed_R);

        // Print range
        kb_gpio_set(EMITTER_SIDES_PORT, EMITTER_SIDES_PIN, GPIO_PIN_RESET);
        KB_DEBUG_MSG("left range: %d\n", range_L);
        KB_DEBUG_MSG("right range: %d\n", range_R);


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

static void disp_test1(void)
{
    hcms_290x_matrix("TST1");
}

static void disp_test2(void)
{
    hcms_290x_matrix("TST2");
}

static void disp_test3(void)
{
    hcms_290x_matrix("TST3");
}

static void disp_test4(void)
{
    hcms_290x_matrix("TST4");
}

static void search_to_goal(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // wait for button pressed
    while (!is_b2_pressed) {
    }
    is_b1_pressed = 0;

    hcms_290x_matrix("Go");
    kb_delay_ms(1000);
    hcms_290x_matrix("    ");

    /* Motor test running */
    system_enable_range_finder();

    /* Go to main algorithm */
    main_run();

    /* Motor test running done */
    system_stop_driving();
    system_disable_range_finder();
    pid_reset(&g_pid_T);
    main_fsm(eol);
}

static void rush_to_goal(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    kb_delay_ms(2000);
    main_fsm(eol);
}

static void print_mem(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    kb_delay_ms(2000);
    main_fsm(eol);
}

static void reset_mem(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    kb_delay_ms(2000);
    main_fsm(eol);
}

static void run_test1_time(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // TODO: stop blinky here
    hcms_290x_matrix("Go");
    kb_delay_ms(500);
    hcms_290x_matrix("    ");

    /* Motor test running */
    system_enable_range_finder();
    system_start_driving();
    pid_input_setpoint(&g_pid_T, 18);

    kb_delay_ms(444000);
    /* Motor test running done */
    system_stop_driving();
    system_disable_range_finder();
    pid_reset(&g_pid_T);
    pid_reset(&g_pid_R);

    main_fsm(eol);
}

static void run_test1_distance(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // TODO: stop blinky here
    hcms_290x_matrix("Go");
    kb_delay_ms(500);
    hcms_290x_matrix("    ");

    /* Motor test running */
    motion_cmd_t cmd;
    for (int i = 0; i < 6; i++) {
        cmd.type = motion_cmd_t::straight;
        cmd.unit = 1;
        cmd.on_start = NULL;
        cmd.on_completed = NULL;
        if (i == 5) {
            // Put an event at the end of moving
            cmd.on_completed = on_task_done;
        }
        motion_queue(&cmd);
    }

    // Sleep (wait) until task is done
    xSemaphoreTake(semphr_task, portMAX_DELAY);

    main_fsm(eol);
}

static void run_test2_CW(void)
{
    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // TODO: stop blinky here
    hcms_290x_matrix("Go");
    kb_delay_ms(500);
    hcms_290x_matrix("    ");

    /* Motor test running */
    motion_cmd_t cmd;
    for (int i = 0; i < 2; i++) {
        cmd.type = motion_cmd_t::turn;
        cmd.unit = -1;
        cmd.on_start = NULL;
        cmd.on_completed = NULL;
        if (i == 1) {
            // Put an event at the end of moving
            cmd.on_completed = on_task_done;
        }
        motion_queue(&cmd);
    }

    // Sleep (wait) until task is done
    xSemaphoreTake(semphr_task, portMAX_DELAY);

    main_fsm(eol);
}

static void run_test2_CCW(void)
{

    /* Give time to get ready */
    hcms_290x_matrix("RDY.");
    for (int i = 0; i < 6; i++) {
        kb_gpio_toggle(LED2_PORT, LED2_PIN);
        kb_delay_ms(500);
    }
    // TODO: stop blinky here
    hcms_290x_matrix("Go");
    kb_delay_ms(500);
    hcms_290x_matrix("    ");

    /* Motor test running */
    motion_cmd_t cmd;
    for (int i = 0; i < 2; i++) {
        cmd.type = motion_cmd_t::turn;
        cmd.unit = 1;
        cmd.on_start = NULL;
        cmd.on_completed = NULL;
        if (i == 1) {
            // Put an event at the end of moving
            cmd.on_completed = on_task_done;
        }
        motion_queue(&cmd);
    }

    // Sleep (wait) until task is done
    xSemaphoreTake(semphr_task, portMAX_DELAY);

    main_fsm(eol);
}

static void run_test3_walls(void)
{
    hcms_290x_matrix("Range");
    kb_delay_ms(2000);

    system_enable_range_finder();
    static volatile uint16_t * const sensors[] = {
            &range_F, &range_L, &range_R
    };
    static uint16_t detect_range[] = {
            MEASURE_RANGE_F_DETECT,
            MEASURE_RANGE_L_DETECT,
            MEASURE_RANGE_R_DETECT
    };
    static char display[5] = {' ', ' ', ' ', ' ', '\0'};
    is_b2_pressed = 0;
    while (!is_b2_pressed) {
        char *ptr = display;
        for (int i = 0; i < 3 ; i++) {
            *ptr++ = (*sensors[i] > detect_range[i])?'0':'X';
        }
        hcms_290x_matrix(display);
    }

    // end
    is_b2_pressed = 0;
    system_disable_range_finder();
    main_fsm(eol);
}

static void run_test4(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    kb_delay_ms(2000);
    main_fsm(eol);
}

static void do_nothing(void)
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
    xSemaphoreGive(semphr_task);
}
