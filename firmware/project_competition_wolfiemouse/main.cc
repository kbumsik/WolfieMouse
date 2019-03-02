
// FreeRTOS
#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "system_control.h"
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
static void disp_task_1(void);
static void disp_task_2(void);
static void disp_task_3(void);
static void disp_task_4(void);
static void disp_task_5(void);
static void disp_task_6(void);

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

// Task declarations
static void task_main(void *pvParameters);

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
static void task_1(void);
static void task_2(void);
static void task_3(void);
static void task_4(void);
static void task_5(void);
static void task_6(void);

// event enum
enum event {
    b1_pressed, b2_pressed, wheel_up, wheel_down, eol
};

// state enum
enum state {
    state_1, state_2, state_3, state_4, state_5, state_6
};

// transition and table
struct transition {
    enum event event_val;
    void (*task) (void);
    enum state next_state;
};

// stopwatch current state
enum state current_state = state_1;

// fsm function
void main_fsm(enum event event_input){
    static const struct transition state_1_transitions[] = {
    //  Event        Task             Next_state
        {b1_pressed, task_1,          state_1   },
        {b2_pressed, task_1,          state_1   },
        {wheel_up,   disp_task_2,     state_2   },
        {wheel_down, disp_task_6,     state_6   },
        {eol,        disp_task_2,     state_1   }
    };
    static const struct transition state_2_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_2,        state_2     },
        {b2_pressed, task_2,        state_2     },
        {wheel_up,   disp_task_3,   state_3     },
        {wheel_down, disp_task_1,   state_1     },
        {eol,        disp_task_2,   state_2     }
    };
    static const struct transition state_3_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_3,        state_3     },
        {b2_pressed, task_3,        state_3     },
        {wheel_up,   disp_task_4,   state_4     },
        {wheel_down, disp_task_2,   state_2     },
        {eol,        disp_task_3,   state_3     }
    };
    static const struct transition state_4_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_4,        state_4     },
        {b2_pressed, task_4,        state_4     },
        {wheel_up,   disp_task_5,   state_5     },
        {wheel_down, disp_task_3,   state_3     },
        {eol,        disp_task_4,   state_4     }
    };
    static const struct transition state_5_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_5,        state_5     },
        {b2_pressed, task_5,        state_5     },
        {wheel_up,   disp_task_6,   state_6     },
        {wheel_down, disp_task_4,   state_4     },
        {eol,        disp_task_5,   state_5     }
    };
    static const struct transition state_6_transitions[] = {
    //  Event        Task           Next_state
        {b1_pressed, task_6,        state_6     },
        {b2_pressed, task_6,        state_6     },
        {wheel_up,   disp_task_1,   state_1     },
        {wheel_down, disp_task_5,   state_5     },
        {eol,        disp_task_6,   state_6     }
    };
    // FSM table. Since it is const, it will be stored in FLASH
    static const struct transition *fsm_table[6] = {
        state_1_transitions,
        state_2_transitions,
        state_3_transitions,
        state_4_transitions,
        state_5_transitions,
        state_6_transitions,
    };

    // search for signal
    int i;
    for (i = 0; (fsm_table[current_state][i].event_val != event_input)
        && (fsm_table[current_state][i].event_val != eol); i++){
        // Do nothing until hit a transition
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
    // Just to display "TSK1"
    main_fsm(eol);

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

        /* Call this Task every 1000ms */
        //vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_RATE_MS));
    }
    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}

/*******************************************************************************
 * FSM action function
 ******************************************************************************/
static void task_1(void)
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

static void task_2(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
    main_fsm(eol);
}

static void task_3(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
    main_fsm(eol);
}

static void task_4(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
    main_fsm(eol);
}

static void task_5(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
    main_fsm(eol);
}

static void task_6(void)
{
    /* Not implemented */
    hcms_290x_matrix("None");
    delay_ms(2000);
    main_fsm(eol);
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

/*******************************************************************************
 * Task display
 ******************************************************************************/
static void disp_task_1(void)
{
    hcms_290x_matrix("TSK1");
}

static void disp_task_2(void)
{
    hcms_290x_matrix("TSK2");
}

void disp_task_3(void)
{
    hcms_290x_matrix("TSK3");
}

void disp_task_4(void)
{
    hcms_290x_matrix("TSK4");
}

void disp_task_5(void)
{
    hcms_290x_matrix("TSK5");
}

void disp_task_6(void)
{
    hcms_290x_matrix("TSK6");
}