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
#include "cmd.h"

#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*******************************************************************************
 * Control loop thread
 ******************************************************************************/
static void control_loop(void *pvParameters);
static TaskHandle_t control_loop_handler;

static SemaphoreHandle_t semphr_from_isr = NULL;
static QueueHandle_t cmd_queue = NULL;
static SemaphoreHandle_t cmd_semphr = NULL;

/*******************************************************************************
 * Private variables and functions
 ******************************************************************************/
// static void process_cmd(motion_cmd_t *cmd);

void on_motion_completed (void);

// static void (*completed_callback)(void);
// static QueueHandle_t queue_motion;

/*******************************************************************************
 * Static local variables
 ******************************************************************************/
// Target steps
volatile struct _target_step {
    uint32_t left;
    uint32_t right;
} target_step;

// PID handler
struct _pid_struct {
    pid_handler_t tran;
    pid_handler_t rot;
} pid;

// Encoder value
volatile struct encoder_data step, step_old;
volatile struct _speed_struct {
    int32_t left;
    int32_t right;
    int32_t diff;    // left - right
} speed;

/* Set PID controller */
pid_value_t pid_tran_forwarding_value = {
        .kp = 35,
        .ki = 5, // 0.01
        .kd = 10
};

pid_value_t pid_rot_forwarding_value = {
            .kp = 3,
            .ki = 0,
            .kd = 400
};

pid_value_t pid_tran_rotating_value = {
        .kp = 35,
        .ki = 5, // 0.01
        .kd = 10
};

pid_value_t pid_rot_rotating_value = {
            .kp = 7,
            .ki = 0.1,
            .kd = 20
};

pid_value_t pid_tran_smooth_value = {
        .kp = 35,
        .ki = 5, // 0.01
        .kd = 10
};

pid_value_t pid_rot_smooth_value = {
            .kp = 30,
            .ki = 0,
            .kd = 20
};


// Range finder value
volatile struct range_data range;

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
    int cmd_ready;
    enum cmd_type current_cmd;
    enum wheel_state left_wheel;
    enum wheel_state right_wheel;
};

static volatile struct _state state = {
    .hardware = 0,
    .pid = 0,
    .pid_tran = 0,
    .pid_rot = 0,
    .range = 0,
    .cmd_ready = 0,
    .current_cmd = CMD_NOTHING,
    .left_wheel = WHEEL_DISABLED,
    .right_wheel = WHEEL_DISABLED,
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

    pid_set_pid(&pid.tran, &pid_tran_forwarding_value);
    pid_reset(&pid.tran);

    pid_set_pid(&pid.rot, &pid_rot_forwarding_value);
    pid_reset(&pid.rot);

    step.left = MEASURE_ENCODER_DEFAULT;
    step.right = MEASURE_ENCODER_DEFAULT;
    step_old.left = MEASURE_ENCODER_DEFAULT;
    step_old.right = MEASURE_ENCODER_DEFAULT;

    pid_input_setpoint(&pid.tran, 0);
    pid_input_setpoint(&pid.rot, 0);

    /* Now peripherals has been initialized */
    
    /* Allocate Semaphore */
    // Max counting is 1, initial is zero
    semphr_from_isr = xSemaphoreCreateCounting(1, 0);
    /* Allocate Queue */
    cmd_queue = xQueueCreate( 10, sizeof(struct cmd_queue_element));
    if (NULL == cmd_queue) {
        KB_DEBUG_ERROR("Creating cmd queue failed!!");
    }
    cmd_semphr = xSemaphoreCreateCounting(1, 0);
    state.cmd_ready = 1;

    /* Allocate task */
    BaseType_t result;
    result = xTaskCreate(
            control_loop,
            "Cnrt",
            configMINIMAL_STACK_SIZE+2000,
            NULL,
            configMAX_PRIORITIES,
            &control_loop_handler);
    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating motion task failed!!");
    }
}


static void control_loop(void *pvParameters)
{
    static struct cmd_queue_element cmd;
    system_enable_range_finder();

    while (1) {
        // Wait from Systick
        // taskYIELD();
        xSemaphoreTake(semphr_from_isr, portMAX_DELAY);

        // Get a cmd
        int cmd_updated = 0;
        if (state.cmd_ready) {
            if (xQueueReceive(cmd_queue, &cmd, 0)) {
                cmd_updated = 1;
                state.cmd_ready = 0;
                state.current_cmd = cmd.type;
                
                // decode and apply command
                switch(cmd.type) {
                    /* Low Level commands */
                    case CMD_LOW_PID_AND_GO:
                        pid_input_setpoint(&pid.tran, cmd.pid.setpoint_trans);
                        pid_input_setpoint(&pid.rot, cmd.pid.setpoint_rot);
                        system_enable_range_finder();
                        system_start_driving();
                        state.cmd_ready = 1;
                        state.current_cmd = CMD_NOTHING;
                    break;
                    case CMD_LOW_PID_RESET_AND_STOP:
                        system_stop_driving();
                        system_disable_range_finder();
                        pid_reset(&pid.tran);
                        pid_reset(&pid.rot);
                        state.cmd_ready = 1;
                        state.current_cmd = CMD_NOTHING;
                    break;
                    /* High level commands */
                    case CMD_BACK_TO_SART_CENTER:
                        pid_set_pid(&pid.tran, &pid_tran_forwarding_value);
                        pid_set_pid(&pid.rot, &pid_rot_forwarding_value);

                        pid_input_setpoint(&pid.tran, 10);
                        pid_input_setpoint(&pid.rot, 0);

                        target_step.left = MEASURE_STEPS_BACK_TO_START_CENTER;
                        target_step.left += MEASURE_ENCODER_DEFAULT;
                        target_step.right = MEASURE_STEPS_BACK_TO_START_CENTER;
                        target_step.right += MEASURE_ENCODER_DEFAULT;

                        state.left_wheel = WHEEL_FORWARD;
                        state.right_wheel = WHEEL_FORWARD;

                        system_enable_range_finder();
                        system_start_driving();
                    break;
                    case CMD_F:
                        pid_set_pid(&pid.tran, &pid_tran_forwarding_value);
                        pid_set_pid(&pid.rot, &pid_rot_forwarding_value);

                        pid_input_setpoint(&pid.tran, 10);
                        pid_input_setpoint(&pid.rot, 0);

                        target_step.left = MEASURE_STEPS_PER_CELL;
                        target_step.left += MEASURE_ENCODER_DEFAULT;
                        target_step.right = MEASURE_STEPS_PER_CELL;
                        target_step.right += MEASURE_ENCODER_DEFAULT;

                        state.left_wheel = WHEEL_FORWARD;
                        state.right_wheel = WHEEL_FORWARD;

                        system_enable_range_finder();
                        system_start_driving();
                    break;
                    case CMD_H_F:
                        pid_set_pid(&pid.tran, &pid_tran_forwarding_value);
                        pid_set_pid(&pid.rot, &pid_rot_forwarding_value);

                        pid_input_setpoint(&pid.tran, 10);
                        pid_input_setpoint(&pid.rot, 0);

                        target_step.left = MEASURE_STEPS_PER_CELL / 2;
                        target_step.left += MEASURE_ENCODER_DEFAULT;
                        target_step.right = MEASURE_STEPS_PER_CELL / 2;
                        target_step.right += MEASURE_ENCODER_DEFAULT;
                        
                        state.left_wheel = WHEEL_FORWARD;
                        state.right_wheel = WHEEL_FORWARD;

                        system_enable_range_finder();
                        system_start_driving();
                    break;
                    case CMD_L:
                        pid_set_pid(&pid.tran, &pid_tran_rotating_value);
                        pid_set_pid(&pid.rot, &pid_rot_rotating_value);
                        pid_reset(&pid.tran);
                        pid_reset(&pid.rot);

                        pid_input_setpoint(&pid.tran, 0);
                        pid_input_setpoint(&pid.rot, -30);
                        
                        target_step.left = -MEASURE_STEPS_90DEG_CCW;
                        target_step.left += MEASURE_ENCODER_DEFAULT;
                        target_step.right = MEASURE_STEPS_90DEG_CCW;
                        target_step.right += MEASURE_ENCODER_DEFAULT;

                        state.left_wheel = WHEEL_BACKWARD;
                        state.right_wheel = WHEEL_FORWARD;

                        system_disable_range_finder();  // MUST BE OFF
                        system_start_driving();
                    break;
                    case CMD_R:
                        pid_set_pid(&pid.tran, &pid_tran_rotating_value);
                        pid_set_pid(&pid.rot, &pid_rot_rotating_value);
                        pid_reset(&pid.tran);
                        pid_reset(&pid.rot);
                        
                        pid_input_setpoint(&pid.tran, 0);
                        pid_input_setpoint(&pid.rot, 30);
                        
                        target_step.left = MEASURE_STEPS_90DEG_CW;
                        target_step.left += MEASURE_ENCODER_DEFAULT;
                        target_step.right = -MEASURE_STEPS_90DEG_CW;
                        target_step.right += MEASURE_ENCODER_DEFAULT;
                        
                        state.left_wheel = WHEEL_FORWARD;
                        state.right_wheel = WHEEL_BACKWARD;

                        system_disable_range_finder();  // MUST BE OFF
                        system_start_driving();
                    break;
                    case CMD_S_L:
                        pid_set_pid(&pid.tran, &pid_tran_smooth_value);
                        pid_set_pid(&pid.rot, &pid_rot_smooth_value);

                        pid_input_setpoint(&pid.tran, 15);
                        pid_input_setpoint(&pid.rot, -30);
                        
                        target_step.left = MEASURE_STEPS_SMOOTH_L_LEFT;
                        target_step.left += MEASURE_ENCODER_DEFAULT;
                        target_step.right = MEASURE_STEPS_SMOOTH_L_RIGHT;
                        target_step.right += MEASURE_ENCODER_DEFAULT;

                        state.left_wheel = WHEEL_FORWARD;
                        state.right_wheel = WHEEL_FORWARD;

                        system_disable_range_finder();  // MUST BE OFF
                        system_start_driving();
                    break;
                    case CMD_S_R:
                        pid_set_pid(&pid.tran, &pid_tran_smooth_value);
                        pid_set_pid(&pid.rot, &pid_rot_smooth_value);

                        pid_input_setpoint(&pid.tran, 15);
                        pid_input_setpoint(&pid.rot, 30);
                        
                        target_step.left = MEASURE_STEPS_SMOOTH_R_LEFT;
                        target_step.left += MEASURE_ENCODER_DEFAULT;
                        target_step.right = MEASURE_STEPS_SMOOTH_R_RIGHT;
                        target_step.right += MEASURE_ENCODER_DEFAULT;

                        state.left_wheel = WHEEL_FORWARD;
                        state.right_wheel = WHEEL_FORWARD;

                        system_disable_range_finder();  // MUST BE OFF
                        system_start_driving();
                    break;
                    default:
                        state.cmd_ready = 1;
                        state.current_cmd = CMD_NOTHING;
                    break;
                    // case CMD_S_L:
                    // break;
                    // case CMD_S_R:
                    // break;
                }
            } else {
                cmd_updated = 0;
            }
        } else {
            cmd_updated = 0;
        }

        step_old = step;
        encoder_get(&step, ENCODER_CH_BOTH);
        // update speed
        speed.left = (step.left - step_old.left);// * CONFIG_LEN_PER_CNT;
        speed.right = (step.right - step_old.right);// * CONFIG_LEN_PER_CNT;
        speed.diff = speed.left - speed.right;

        if (cmd_updated) {
            // Reset steps and encoders
            encoder_reset(ENCODER_CH_BOTH);
            step.left = MEASURE_ENCODER_DEFAULT;
            step.right = MEASURE_ENCODER_DEFAULT;
        }

        /* Start PID calculation */
        // calculate errorT
        int32_t feedback_T = (speed.left + speed.right) / 2;
        int32_t outputT = pid_compute(&pid.tran, feedback_T);

        // Get ADC values
        if(state.range) {
            range_get(&range, RANGE_CH_ALL);
        }

        // if it is too close stop
        if ((range.front > MEASURE_RANGE_F_NEAR_DETECT) ||
        (range.front_right > MEASURE_RANGE_F_NEAR_DETECT))
        {
            if ((cmd.type != CMD_L) && (cmd.type != CMD_R)){
                system_start_driving();
                state.cmd_ready = 1;
                pid_reset(&pid.tran);
                pid_reset(&pid.rot);
                // Send a notification
                xSemaphoreGive(cmd_semphr);
                continue;
            }
        }

        if (range.left > MEASURE_RANGE_L_MAX_DETECT) {
            range.left = MEASURE_RANGE_L_MAX_DETECT;
        }

        if (range.right > MEASURE_RANGE_R_MAX_DETECT) {
            range.right = MEASURE_RANGE_R_MAX_DETECT;
        }

        // get errorR
        int32_t feedback_R;
        if(state.range && (range.left > (MEASURE_RANGE_L_MIN_DETECT))
                            && (range.right > MEASURE_RANGE_R_MIN_DETECT) ) {
            // If both range are within wall detecting distance,
            // Use range sensor to get rotational error
            feedback_R = (range.right - range.left - MEASURE_RANGE_R_OFFSET) / 10;
        } else if (state.range && range.left > (MEASURE_RANGE_L_MIN_DETECT + 50)) {
            // If only left side is within range
            // use the middle value of the right range
            feedback_R = (MEASURE_RANGE_R_M_DETECT - range.left) / 10;
        } else if (state.range && range.right > (MEASURE_RANGE_R_MIN_DETECT + 50)) {
            // If only right side is within range
            // use the middle value of the left range
            feedback_R = (range.right - MEASURE_RANGE_L_M_DETECT) / 10;
        } else {
            // in open space, use rotary encoder
            feedback_R = speed.diff;
        }

        int32_t outputR = pid_compute(&pid.rot, feedback_R);

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

        // check tartget steps status
        if (!state.cmd_ready) {
            // update both wheels
            // left
            if (state.left_wheel == WHEEL_FORWARD) {
                if (step.left > target_step.left) {
                    state.left_wheel = WHEEL_DISABLED;
                }
            } else if (state.left_wheel == WHEEL_BACKWARD) {
                if (step.left < target_step.left) {
                    state.left_wheel = WHEEL_DISABLED;
                }
            }

            // right
            if (state.right_wheel == WHEEL_FORWARD) {
                if (step.right > target_step.right) {
                    state.right_wheel = WHEEL_DISABLED;
                }
            } else if (state.right_wheel == WHEEL_BACKWARD) {
                if (step.right < target_step.right) {
                    state.right_wheel = WHEEL_DISABLED;
                }
            }

            // if both wheels are completed then a command is complete
            if ((state.left_wheel == WHEEL_DISABLED) && (state.right_wheel == WHEEL_DISABLED)) {
                state.cmd_ready = 1;
                // state.current_cmd = CMD_NOTHING;
                if ((state.current_cmd == CMD_R) || (state.current_cmd == CMD_L)) {
                    pid_reset(&pid.tran);
                    pid_reset(&pid.rot);
                }
                // Send a notification
                xSemaphoreGive(cmd_semphr);
            }
        }

        // Print the result in json format
        // terminal_puts("{");
        // terminal_printf("\"L\":%d,", range.left);
        // terminal_printf("\"R\":%d,", range.right);
        // terminal_printf("\"F\":%d", range.front);
        // // terminal_printf("\"LS\":%d,", speed.left);
        // // terminal_printf("\"RS\":%d,", speed.right);
        // // terminal_printf("\"LO\":%d,", outputT + outputR);
        // // terminal_printf("\"RO\":%d,", outputT - outputR);
        // // terminal_printf("\"TTS\":%d,", pid.tran.setpoint);
        // // terminal_printf("\"TRS\":%d", pid.rot.setpoint);
        // // terminal_printf("\"T\":%d", tick_us() - time);
        // terminal_puts("},\n");
    }

    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}


QueueHandle_t thread_control_loop_cmd_queue(void)
{
    return cmd_queue;
}

SemaphoreHandle_t thread_control_loop_cmd_semphr(void)
{
    return cmd_semphr;
}
// static void process_cmd(motion_cmd_t *cmd)
// {
//     static int32_t target_pid_T;
//     static int32_t target_pid_R;
//     static int32_t target_step.left;
//     static int32_t target_step.right;
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
//         target_step.left = MEASURE_STEPS_PER_CELL * cmd->unit;
//         target_step.right = MEASURE_STEPS_PER_CELL * cmd->unit;
//         break;
//     case turn:
//         // Disable the range finder
//         system_disable_range_finder();
//         // set PID
//         target_pid_T = 0;
//         if (cmd->unit >= 0) {
//             target_pid_R = -60;
//             target_step.left = -MEASURE_STEPS_90DEG_CCW * cmd->unit;
//             target_step.right = MEASURE_STEPS_90DEG_CCW * cmd->unit;
//         } else {
//             target_pid_R = 60;
//             target_step.left = -MEASURE_STEPS_90DEG_CW * cmd->unit;
//             target_step.right = MEASURE_STEPS_90DEG_CW * cmd->unit;
//         }
//         break;
//     case curve:
//         // Not implemented yet.
//         break;
//     default:
//         break;
//     }
//     // apply new values
//     pid_input_setpoint(&pid.tran, target_pid_T);
//     pid_input_setpoint(&pid.rot, target_pid_R);
//     // leftforward_L();
//     } else {
//         system_motion_backward_L();
//     }
//     // right
//     g_motion_target_R = MEASURE_ENCODER_DEFAULT + target_step.right;
//     if (target_step.right >= 0) {
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
//             pid_reset(&pid.tran);
//             pid_reset(&pid.rot);
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
