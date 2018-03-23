/*
 * motion_controller.c
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#include "system_config.h"
// #include "motion_controller.h"
#include "system_control.h"
#include "common_source.h"
#include "encoder.h"
#include "gpio.h"
#include "motor.h"
#include "range.h"
#include "terminal.h"
#include "hcms_290x_display.h"

#include "config_measurements.h"
#include "pid.h"
#include "tick.h"

#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*******************************************************************************
 * Global variables import
 ******************************************************************************/
// PID handler
pid_handler_t g_pid_T;
pid_handler_t g_pid_R;

/*******************************************************************************
 * Global variables
 ******************************************************************************/
uint32_t g_motion_target_L = 0;
uint32_t g_motion_target_R = 0;

/*******************************************************************************
 * Control loop thread
 ******************************************************************************/
static void control_loop(void *pvParameters);
static TaskHandle_t control_loop_handler;

static SemaphoreHandle_t semphr_from_isr = NULL;

/*******************************************************************************
 * Private variables and functions
 ******************************************************************************/
// static void process_cmd(motion_cmd_t *cmd);

void on_motion_completed (void);

// static void (*completed_callback)(void);
// static QueueHandle_t queue_motion;

/*******************************************************************************
 * Global variables
 ******************************************************************************/

// Encoder value
volatile struct encoder_data step, step_old;
volatile struct _speed_struct {
    int32_t left;
    int32_t right;
    int32_t diff;    // left - right
} speed;

// Range finder value
volatile struct range_data range;

// // PID handler
// pid_handler_t g_pid_T;    // Translational
// pid_handler_t g_pid_R; // Rotational

/*******************************************************************************
 * States 
 ******************************************************************************/
enum wheel_state {
    WHEEL_DISABLED, WHEEL_FORWARD, WHEEL_BACKWARD
};

struct _state {
    int hardware;
    int pid;
    int pid_tran;
    int pid_rot;
    int range;
    // enum wheel_state wheel_right;
    // enum wheel_state wheel_left;
};

static volatile struct _state state = {
    .hardware = 0,
    .pid = 0,
    .pid_tran = 0,
    .pid_rot = 0,
    .range = 0,
    // .wheel_right = WHEEL_DISABLED,
    // .wheel_left = WHEEL_DISABLED,
};

/*******************************************************************************
 * Function definition
 ******************************************************************************/
void thread_control_loop_init(void)
{
    gpio_init_t GPIO_InitStruct;

    /* Configure UART */
    terminal_init();
    
    /* Init motor */
    motor_init();
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);


    /* Configure Pushbuttons */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init(B1_PORT, B1_PIN, &GPIO_InitStruct);
    gpio_init(B2_PORT, B2_PIN, &GPIO_InitStruct);

    /* Configure LED pins */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init(LED1_PORT, LED1_PIN, &GPIO_InitStruct);
    gpio_init(LED2_PORT, LED2_PIN, &GPIO_InitStruct);
    gpio_init(LED3_PORT, LED3_PIN, &GPIO_InitStruct);
    gpio_init(LED4_PORT, LED4_PIN, &GPIO_InitStruct);
    gpio_init(LED5_PORT, LED5_PIN, &GPIO_InitStruct);
    gpio_init(LED6_PORT, LED6_PIN, &GPIO_InitStruct);

    /* Configure LED Pins Output Level */
    gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    gpio_set(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_RESET);
    gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_RESET);
    gpio_set(LED5_PORT, LED5_PIN, GPIO_PIN_RESET);
    gpio_set(LED6_PORT, LED6_PIN, GPIO_PIN_RESET);

    /* Init peripherals for the LED display */
    hcms_290x_init();

    /* Range Sensor */
    range_init();
    
    /* Init encoder */
    encoder_init();
    encoder_get(&step, ENCODER_CH_BOTH);

    /* Set PID controller */
    pid_value_t pid_T_value = {
            .kp = 35,
            .ki = 5, // 0.01
            .kd = 10
    };

    pid_value_t pid_R_value = {
                .kp = 1,
                .ki = 0,
                .kd = 2
    };

    pid_set_pid(&g_pid_T, &pid_T_value);
    pid_reset(&g_pid_T);

    pid_set_pid(&g_pid_R, &pid_R_value);
    pid_reset(&g_pid_R);

    pid_input_setpoint(&g_pid_T, 0);
    pid_input_setpoint(&g_pid_R, 0);

    /* Now peripherals has been initialized */
    
    /* Allocate task */
    BaseType_t result;
    result = xTaskCreate(
            control_loop,
            "Mot",
            configMINIMAL_STACK_SIZE+2000,
            NULL,
            configMAX_PRIORITIES,
            &control_loop_handler);
    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating motion task failed!!");
    }

    // /* Allocate Queue */
    // queue_motion = xQueueCreate( 10, sizeof(motion_cmd_t));
    // if (queue_motion == NULL) {
    //     KB_DEBUG_ERROR("Creating motion queue failed!!");
    // }

    /* Allocate Semaphore */
    // Max counting is 1, initial is zero
    semphr_from_isr = xSemaphoreCreateCounting(1, 0);
}


static void control_loop(void *pvParameters)
{
    system_enable_range_finder();

    while (1) {
        // Wait from Systick
        // taskYIELD();
        xSemaphoreTake(semphr_from_isr, portMAX_DELAY);

        uint32_t time = tick_us();

        // Get encoder values and calculate speed
        step_old = step;
        encoder_get(&step, ENCODER_CH_BOTH);

        speed.left = (step.left - step_old.left);// * CONFIG_LEN_PER_CNT;
        speed.right = (step.right - step_old.right);// * CONFIG_LEN_PER_CNT;
        speed.diff = speed.left - speed.right;

        // /* Motion controller notification */
        // for (int i = 0; i < 2; i++) {
        //     // Change variables being checked
        //     static enum motion_status *status;
        //     static int32_t steps;
        //     static uint32_t target;
        //     if (i == 0) {
        //         status = &state.wheel_left;
        //         steps = step.left;
        //         target = g_motion_target_L;
        //     } else {
        //         status = &state.wheel_right;
        //         steps = step.right;
        //         target = g_motion_target_R;
        //     }
        //     // check variables
        //     if (disabled != *status) {
        //         if ((forward == *status) &&
        //                 (steps >= target)) {
        //             *status = disabled;
        //             xSemaphoreGiveFromISR(g_motion_semphr, &xHigherPriorityTaskWoken);
        //         } else if ((backward == *status) &&
        //                 (steps <= target)) {
        //             *status = disabled;
        //             xSemaphoreGiveFromISR(g_motion_semphr, &xHigherPriorityTaskWoken);
        //         }
        //     }
        // }

        /* Start PID calculation */
        // calculate errorT
        int32_t feedback_T = (speed.left + speed.right) / 2;
        int32_t outputT = pid_compute(&g_pid_T, feedback_T);

        // Get ADC values
        if(state.range) {
            range_get(&range, RANGE_CH_ALL);
        }

        // get errorR
        int32_t feedback_R;
        if(state.range && (range.left > MEASURE_RANGE_L_DETECT)
                            && (range.right > MEASURE_RANGE_R_DETECT) ) {
            // If both range are within wall detecting distance,
            // Use range sensor to get rotational error
            feedback_R = (range.right - range.left - MEASURE_RANGE_R_OFFSET) / 10;
        } else if (state.range && range.left > MEASURE_RANGE_L_DETECT) {
            // If only left side is within range
            // use the middle value of the right range
            feedback_R = (MEASURE_RANGE_R_MIDDLE - range.left) / 10;
        } else if (state.range && range.right > MEASURE_RANGE_R_DETECT) {
            // If only right side is within range
            // use the middle value of the left range
            feedback_R = (range.right - MEASURE_RANGE_L_MIDDLE) / 10;
        } else {
            // in open space, use rotary encoder
            feedback_R = speed.diff * 10;
        }

        int32_t outputR = pid_compute(&g_pid_R, feedback_R);

        if (!state.pid_tran) {
            outputT = 0;
        }
        if (!state.pid_rot) {
            outputR = 0;
        }
        if (!state.pid) {
            continue;
        }

        // Apply to the motor
        motor_speed_permyriad(CH_LEFT, outputT + outputR);
        motor_speed_permyriad(CH_RIGHT, outputT - outputR);
        motor_start(CH_BOTH);

        // Print the result in json format
        terminal_puts("{");
        terminal_printf("\"LS\":%d,", speed.left);
        terminal_printf("\"RS\":%d,", speed.right);
        terminal_printf("\"LO\":%d,", outputT + outputR);
        terminal_printf("\"RO\":%d,", outputT - outputR);
        terminal_printf("\"TTS\":%d,", g_pid_T.setpoint);
        terminal_printf("\"TRS\":%d,", g_pid_R.setpoint);
        terminal_printf("\"T\":%d", tick_us() - time);
        terminal_puts("},\n");
    }

    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}

// void motion_queue(motion_cmd_t *commend)
// {
//     xQueueSend(queue_motion, commend, portMAX_DELAY);
// }

// static void process_cmd(motion_cmd_t *cmd)
// {
//     static int32_t target_pid_T;
//     static int32_t target_pid_R;
//     static int32_t target_steps_L;
//     static int32_t target_steps_R;
//     // reset encoder first
//     system_reset_encoder();

//     // Set target steps and pid
//     switch (cmd->type) {
//     case straight:
//         // Enable the range finder
//         system_enable_range_finder();
//         // set PID
//         target_pid_T = 18;
//         target_pid_R = 0;
//         // Set target steps
//         target_steps_L = MEASURE_STEPS_PER_CELL * cmd->unit;
//         target_steps_R = MEASURE_STEPS_PER_CELL * cmd->unit;
//         break;
//     case turn:
//         // Disable the range finder
//         system_disable_range_finder();
//         // set PID
//         target_pid_T = 0;
//         if (cmd->unit >= 0) {
//             target_pid_R = -60;
//             target_steps_L = -MEASURE_STEPS_90DEG_CCW * cmd->unit;
//             target_steps_R = MEASURE_STEPS_90DEG_CCW * cmd->unit;
//         } else {
//             target_pid_R = 60;
//             target_steps_L = -MEASURE_STEPS_90DEG_CW * cmd->unit;
//             target_steps_R = MEASURE_STEPS_90DEG_CW * cmd->unit;
//         }
//         break;
//     case curve:
//         // Not implemented yet.
//         break;
//     default:
//         break;
//     }
//     // apply new values
//     pid_input_setpoint(&g_pid_T, target_pid_T);
//     pid_input_setpoint(&g_pid_R, target_pid_R);
//     // left
//     g_motion_target_L = MEASURE_ENCODER_DEFAULT + target_steps_L;
//     if (target_steps_L >= 0) {
//         system_motion_forward_L();
//     } else {
//         system_motion_backward_L();
//     }
//     // right
//     g_motion_target_R = MEASURE_ENCODER_DEFAULT + target_steps_R;
//     if (target_steps_R >= 0) {
//         system_motion_forward_R();
//     } else {
//         system_motion_backward_R();
//     }
//     // Register on_completed callback function
//     completed_callback = cmd->on_completed;

//     // Invoke the on_start callback function.
//     if (NULL != cmd->on_start) {
//         cmd->on_start();
//     }
// }

// static void task_motion(void *pvParameters)
// {
//     motion_cmd_t cmd;
//     int cmd_available = 0;
//     while (1) {
//         xQueueReceive(queue_motion, &cmd, portMAX_DELAY);
//         if (!cmd_available) {
//             // If it is the first run after turn off motors,
//             // power up motors
//             cmd_available = 1;
//             system_start_driving();
//         }
//         // Process the next commend
//         process_cmd(&cmd);
//         // wait for motion completed.
//         // Take semaphore TWO times, left and right.
//         xSemaphoreTake(g_motion_semphr, portMAX_DELAY);
//         xSemaphoreTake(g_motion_semphr, portMAX_DELAY);

//         // Invoke callback
//         on_motion_completed();

//         // Check if there is message wating
//         cmd_available = uxQueueMessagesWaiting(queue_motion);
//         KB_DEBUG_MSG("Motion available: %d\r\n", cmd_available);
//         if (0 == cmd_available){
//             //turn of motors
//             system_stop_driving();
//             system_disable_range_finder();
//             pid_reset(&g_pid_T);
//             pid_reset(&g_pid_R);
//         }
//     }
// }

// void on_motion_completed (void)
// {
//     // call callback
//     if (completed_callback != NULL) {
//         completed_callback();
//     }
// }



void SysTick_hook(void)
{
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (semphr_from_isr != NULL) {
        xSemaphoreGiveFromISR(semphr_from_isr, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


/*******************************************************************************
 * system functions from @system_control.h
 ******************************************************************************/
// Motor driving
void system_start_driving(void)
{
    state.pid = 1;
    state.pid_tran = 1;
    state.pid_rot = 1;
}

void system_stop_driving(void)
{
    state.pid = 0;
    state.pid_tran = 0;
    state.pid_rot = 0;
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);
}

// Range finder control
void system_enable_range_finder(void)
{
    state.range = 1;
}

void system_disable_range_finder(void)
{
    state.range = 0;
}

void system_reset_encoder(void)
{
    /* before reset hardware, reset old steps */
    // first compansate values before resetting
    encoder_get(&step, ENCODER_CH_BOTH); 
    step_old.left = MEASURE_ENCODER_DEFAULT - (step.left - step_old.left);
    step_old.right = MEASURE_ENCODER_DEFAULT - (step.right - step_old.right);
    encoder_reset(ENCODER_CH_BOTH);
}