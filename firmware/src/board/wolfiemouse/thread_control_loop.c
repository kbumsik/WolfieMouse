/*
 * motion_controller.c
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#include "system_config.h"
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

#ifdef THREAD_CONTROL_LOOP_PRINT_DATA
    #warning THREAD_CONTROL_LOOP_PRINT_DATA is defined, it will print dat for \
             the control loop every 1ms. Turn this off for performance.
#endif

/*******************************************************************************
 * Control loop thread
 ******************************************************************************/
static void control_loop(void *pvParameters);
static TaskHandle_t control_loop_handler;

static SemaphoreHandle_t semphr_from_isr = NULL;
static QueueHandle_t request_queue = NULL;
static QueueHandle_t response_queue = NULL;


/*******************************************************************************
 * Static local variables
 ******************************************************************************/
// Target steps
struct _target_step {
    uint32_t left;
    uint32_t right;
} target_step;

// PID handler
struct _pid_struct {
    pid_handler_t tran;
    pid_handler_t rot;
} pid;

// Encoder value
struct encoder_data step, step_old;
struct _speed_struct {
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
struct range_data range;

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

static struct _state state = {
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
 * Private Function declarations.
 ******************************************************************************/
// Motor driving
static void start_driving(void);
static void stop_driving(void);

// Range finder control
static void enable_range_finder(void);
static void disable_range_finder(void);

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
    request_queue = xQueueCreate( 10, sizeof(struct cmd_request));
    if (NULL == request_queue) {
        KB_DEBUG_ERROR("Creating cmd queue failed!!");
    }
    response_queue = xQueueCreate( 10, sizeof(struct cmd_response));
    if (NULL == response_queue) {
        KB_DEBUG_ERROR("Creating cmd queue failed!!");
    }
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
    static struct cmd_request cmd;
    static struct cmd_response response;
    int32_t feedback_R = 0;
    int32_t feedback_T = 0;
    int32_t outputT = 0;
    int32_t outputR = 0;

    enable_range_finder();

    while (1) {
        // Wait from Systick
        // taskYIELD();
        xSemaphoreTake(semphr_from_isr, portMAX_DELAY);

#ifdef THREAD_CONTROL_LOOP_PRINT_DATA
        uint32_t loop_start_time = tick_us();
#endif
        // Get a cmd
        int cmd_updated = 0;
        if (state.cmd_ready) {
            if (xQueueReceive(request_queue, &cmd, 0)) {
                cmd_updated = 1;
                state.cmd_ready = 0;
                state.current_cmd = cmd.type;
                
                // decode and apply command
                switch(cmd.type) {
                    /* Low Level commands */
                    case CMD_LOW_PID_AND_GO:
                        pid_input_setpoint(&pid.tran, cmd.pid.setpoint_trans);
                        pid_input_setpoint(&pid.rot, cmd.pid.setpoint_rot);
                        enable_range_finder();
                        start_driving();
                        state.cmd_ready = 1;
                        state.current_cmd = CMD_NOTHING;
                    break;
                    case CMD_LOW_PID_RESET_AND_STOP:
                        stop_driving();
                        disable_range_finder();
                        pid_reset(&pid.tran);
                        pid_reset(&pid.rot);
                        state.cmd_ready = 1;
                        state.current_cmd = CMD_NOTHING;
                    break;
                    case CMD_RANGE_VALUES:
                        response = (struct cmd_response) {
                            .type = CMD_RANGE_VALUES,
                            .range = range,
                        };
                        goto continue_response;
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

                        enable_range_finder();
                        start_driving();
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

                        enable_range_finder();
                        start_driving();
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

                        enable_range_finder();
                        start_driving();
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

                        disable_range_finder();  // MUST BE OFF
                        start_driving();
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

                        disable_range_finder();  // MUST BE OFF
                        start_driving();
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

                        disable_range_finder();  // MUST BE OFF
                        start_driving();
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

                        disable_range_finder();  // MUST BE OFF
                        start_driving();
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
        feedback_T = (speed.left + speed.right) / 2;
        outputT = pid_compute(&pid.tran, feedback_T);

        // Get ADC values
        if(state.range) {
            range_get(&range, RANGE_CH_ALL);
        }

        // if it is too close stop
        if ((range.front > MEASURE_RANGE_F_NEAR_DETECT) ||
        (range.front_right > MEASURE_RANGE_F_NEAR_DETECT))
        {
            if ((cmd.type != CMD_L) && (cmd.type != CMD_R)){
                start_driving();
                state.cmd_ready = 1;
                pid_reset(&pid.tran);
                pid_reset(&pid.rot);
                // Send a notification
                goto continue_empty_response;
            }
        }

        if (range.left > MEASURE_RANGE_L_MAX_DETECT) {
            range.left = MEASURE_RANGE_L_MAX_DETECT;
        }

        if (range.right > MEASURE_RANGE_R_MAX_DETECT) {
            range.right = MEASURE_RANGE_R_MAX_DETECT;
        }

        // get errorR
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

        outputR = pid_compute(&pid.rot, feedback_R);

        if (!state.pid_tran) {
            outputT = 0;
        }
        if (!state.pid_rot) {
            outputR = 0;
        }
        if (!state.pid) {
            goto continue_without_response;
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
                goto continue_empty_response;
            }
        }
        goto continue_without_response;
        
continue_empty_response:
        response = (struct cmd_response) {
            .type = cmd.type,
            .range = (struct range_data){0},
        };
continue_response:
        // Send a notification
        if (xQueueSend(response_queue, &response, 0) != pdPASS) {
            // failed to send a request
            // TODO: when failed?
        }
continue_without_response:
        // Print the result in json format
#ifdef THREAD_CONTROL_LOOP_PRINT_DATA
        terminal_puts("{");
        terminal_printf("\"L\":%d,", range.left);
        terminal_printf("\"R\":%d,", range.right);
        terminal_printf("\"F\":%d", range.front);
        terminal_printf("\"LS\":%ld,", speed.left);
        terminal_printf("\"RS\":%ld,", speed.right);
        terminal_printf("\"LO\":%ld,", outputT + outputR);
        terminal_printf("\"RO\":%ld,", outputT - outputR);
        terminal_printf("\"TTS\":%ld,", pid.tran.setpoint);
        terminal_printf("\"TRS\":%ld", pid.rot.setpoint);
        terminal_printf("\"T\":%lu", tick_us() - loop_start_time);
        terminal_puts("},\n");
#endif
        continue;
    }

    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}


QueueHandle_t thread_control_loop_request_queue(void)
{
    return request_queue;
}

QueueHandle_t thread_control_loop_response_queue(void)
{
    return response_queue;
}

void SysTick_hook(void)
{
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (semphr_from_isr != NULL) {
        xSemaphoreGiveFromISR(semphr_from_isr, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


/*******************************************************************************
 * system functions from @control.h
 ******************************************************************************/
// Motor driving
static void start_driving(void)
{
    state.pid = 1;
    state.pid_tran = 1;
    state.pid_rot = 1;
}

static void stop_driving(void)
{
    state.pid = 0;
    state.pid_tran = 0;
    state.pid_rot = 0;
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);
}

// Range finder control
static void enable_range_finder(void)
{
    state.range = 1;
}

static void disable_range_finder(void)
{
    state.range = 0;
}
