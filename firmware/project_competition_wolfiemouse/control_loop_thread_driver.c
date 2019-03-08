#include "control_loop_thread.h"
#include "control_loop_thread_driver.h"
#include "config_measurements.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "range.h"
#include "logger_thread.h"

/*******************************************************************************
 * Constants definition
 ******************************************************************************/

/**
 * @brief Wheel measurements
 */
#define MEASURE_STEPS_PER_REV       5760
#define MEASURE_STEPS_PER_CELL      7875
#define MEASURE_STEPS_90DEG_CW      2618
#define MEASURE_STEPS_90DEG_CCW     2659
#define MEASURE_STEPS_SMOOTH_L_LEFT     4376
#define MEASURE_STEPS_SMOOTH_L_RIGHT    9258
#define MEASURE_STEPS_SMOOTH_R_LEFT     9258
#define MEASURE_STEPS_SMOOTH_R_RIGHT    4376
#define MEASURE_STEPS_BACK_TO_START_CENTER 1500 //< start is from 3.7cm

/**
 * @brief Reange finder values
 */
#define MEASURE_RANGE_L_MIN_DETECT	660
#define MEASURE_RANGE_L_M_DETECT    1150
#define MEASURE_RANGE_L_MAX_DETECT	1450

#define MEASURE_RANGE_R_MIN_DETECT	900
#define MEASURE_RANGE_R_M_DETECT	1550
#define MEASURE_RANGE_R_MAX_DETECT	1700

#define MEASURE_RANGE_R_OFFSET 300 /* the offset between left and right sensor when mouse in the middle of cell */

/* Set target PID speed */
#define TARGET_MOVE_FORWARD_SPEED_TRAN  (10)
#define TARGET_MOVE_FORWARD_SPEED_ROT   (0)

#define TARGET_PIVOT_SPEED_TRAN         (0)
#define TARGET_PIVOT_SPEED_ROT          (-30)

#define TARGET_SMOOTH_TURN_SPEED_TRAN   (15)
#define TARGET_SMOOTH_TURN_SPEED_ROT    (-30)

/* Set PID controller */
pid_value_t pid_tran_forwarding_value = {
        .kp = 35,
        .ki = 5, // 0.01
        .kd = 10
};

pid_value_t pid_rot_forwarding_value = {
            .kp = 8,
            .ki = 0,
            .kd = 1000
};

pid_value_t pid_tran_rotating_value = {
        .kp = 35,
        .ki = 5, // 0.01
        .kd = 10
};

pid_value_t pid_rot_rotating_value = {
            .kp = 8 ,
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

/*******************************************************************************
 * Local data structure
 ******************************************************************************/
// Wheel state
enum wheel_direction {
    WHEEL_DISABLED, WHEEL_FORWARD, WHEEL_BACKWARD
};

struct wheel_status {
    enum wheel_direction left;
    enum wheel_direction right;
};

/*******************************************************************************
 * Driver and function declarations
 ******************************************************************************/
void do_nothing (struct mouse_data_pid *pid);
void move_from_back_to_start_center (struct mouse_data_pid *pid);
void move_forward_one_cell (struct mouse_data_pid *pid);
void move_forward_half_cell (struct mouse_data_pid *pid);
void pivot_left_90_degree (struct mouse_data_pid *pid);
void pivot_right_90_degree (struct mouse_data_pid *pid);
void turn_left_smooth (struct mouse_data_pid *pid);
void turn_right_smooth (struct mouse_data_pid *pid);
void set_pid_and_go (struct mouse_data_pid *pid);
void reset_pid_and_stop (struct mouse_data_pid *pid);
void send_range_data (struct mouse_data_pid *pid);

void (*const control_loop_driver[])(struct mouse_data_pid *pid) = {
    do_nothing,                     // CMD_NOTHING
    move_from_back_to_start_center, // CMD_BACK_TO_SART_CENTER
    move_forward_one_cell,          // CMD_MOVE_FORWARD_ONE_CELL
    move_forward_half_cell,         // CMD_MOVE_FORWARD_HALF_CELL
    pivot_left_90_degree,           // CMD_PIVOT_LEFT_90_DEGREE
    pivot_right_90_degree,          // CMD_PIVOT_RIGHT_90_DEGREE
    turn_left_smooth,               // CMD_TURN_LEFT_SMOOTH
    turn_right_smooth,              // CMD_TURN_RIGHT_SMOOTH
    set_pid_and_go,                 // CMD_NONBLOCK_SET_PID_AND_GO
    reset_pid_and_stop,             // CMD_NONBLOCK_RESET_PID_AND_STOP
    send_range_data,                // CMD_SENSOR_GET_RANGE
};

/*******************************************************************************
 * Common function definitions
 ******************************************************************************/
/* Control loops */
void loop_move_forward (struct mouse_data_pid *pid,
                        pid_value_t *pid_tran,
                        pid_value_t *pid_rot,
                        int32_t target_speed_tran,
                        int32_t target_speed_rot,
                        int16_t target_step_left,
                        int16_t target_step_right);
void loop_pivot (struct mouse_data_pid *pid,
                 pid_value_t *pid_tran,
                 pid_value_t *pid_rot,
                 int32_t target_speed_tran,
                 int32_t target_speed_rot,
                 int16_t target_step_left,
                 int16_t target_step_right);
void loop_smooth_trun (struct mouse_data_pid *pid,
                       pid_value_t *pid_tran,
                       pid_value_t *pid_rot,
                       int32_t target_speed_tran,
                       int32_t target_speed_rot,
                       int16_t target_step_left,
                       int16_t target_step_right);

/* etc. */
void update_steps_and_speed (struct mouse_data_step *total_step,
                             struct mouse_data_speed *speed);
void update_range (struct range_data *range);
void stop_motor (struct mouse_data_pid *pid);
int check_escape_condition (struct mouse_data_pid *pid,
                            struct mouse_data_step *total_step,
                            struct mouse_data_step *target_step,
                            struct wheel_status *target_wheel_dir);

/*******************************************************************************
 * Function definitions
 ******************************************************************************/
void do_nothing (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void move_from_back_to_start_center (struct mouse_data_pid *pid)
{
    loop_move_forward (pid,
                       &pid_tran_forwarding_value,
                       &pid_rot_forwarding_value,
                       TARGET_MOVE_FORWARD_SPEED_TRAN,
                       TARGET_MOVE_FORWARD_SPEED_ROT,
                       MEASURE_STEPS_BACK_TO_START_CENTER,
                       MEASURE_STEPS_BACK_TO_START_CENTER);
}

void move_forward_one_cell (struct mouse_data_pid *pid)
{
    loop_move_forward (pid,
                       &pid_tran_forwarding_value,
                       &pid_rot_forwarding_value,
                       TARGET_MOVE_FORWARD_SPEED_TRAN,
                       TARGET_MOVE_FORWARD_SPEED_ROT,
                       MEASURE_STEPS_PER_CELL,
                       MEASURE_STEPS_PER_CELL);
}

void move_forward_half_cell (struct mouse_data_pid *pid)
{
    loop_move_forward (pid,
                       &pid_tran_forwarding_value,
                       &pid_rot_forwarding_value,
                       TARGET_MOVE_FORWARD_SPEED_TRAN,
                       TARGET_MOVE_FORWARD_SPEED_ROT,
                       MEASURE_STEPS_PER_CELL / 2,
                       MEASURE_STEPS_PER_CELL / 2);
}

void pivot_left_90_degree (struct mouse_data_pid *pid)
{
    loop_pivot (pid,
                &pid_tran_rotating_value,
                &pid_rot_rotating_value,
                TARGET_PIVOT_SPEED_TRAN,
                TARGET_PIVOT_SPEED_ROT,
                -MEASURE_STEPS_90DEG_CCW,
                MEASURE_STEPS_90DEG_CCW);
}

void pivot_right_90_degree (struct mouse_data_pid *pid)
{
    loop_pivot (pid,
                &pid_tran_rotating_value,
                &pid_rot_rotating_value,
                TARGET_PIVOT_SPEED_TRAN,
                -TARGET_PIVOT_SPEED_ROT,
                MEASURE_STEPS_90DEG_CW,
                -MEASURE_STEPS_90DEG_CW);
}

void turn_left_smooth (struct mouse_data_pid *pid)
{
    loop_smooth_trun (pid,
                      &pid_tran_smooth_value,
                      &pid_rot_smooth_value,
                      TARGET_SMOOTH_TURN_SPEED_TRAN,
                      TARGET_SMOOTH_TURN_SPEED_ROT,
                      MEASURE_STEPS_SMOOTH_L_LEFT,
                      MEASURE_STEPS_SMOOTH_L_RIGHT);
}

void turn_right_smooth (struct mouse_data_pid *pid)
{
    loop_smooth_trun (pid,
                      &pid_tran_smooth_value,
                      &pid_rot_smooth_value,
                      TARGET_SMOOTH_TURN_SPEED_TRAN,
                      -TARGET_SMOOTH_TURN_SPEED_ROT,
                      MEASURE_STEPS_SMOOTH_R_LEFT,
                      MEASURE_STEPS_SMOOTH_R_RIGHT);
}

void set_pid_and_go (struct mouse_data_pid *pid)
{
    // Not used
}

void reset_pid_and_stop (struct mouse_data_pid *pid)
{
    stop_motor(pid);
}

void send_range_data (struct mouse_data_pid *pid)
{
    struct range_data range;
    update_range(&range);
    control_loop_send_response(
        &(struct cmd_response){
            .type = CMD_RESP_RANGE,
            .range = range
        });
}

/*******************************************************************************
 * Common function definitions (control loop)
 ******************************************************************************/
void loop_move_forward (struct mouse_data_pid *pid,
                        pid_value_t *pid_tran,
                        pid_value_t *pid_rot,
                        int32_t target_speed_tran,
                        int32_t target_speed_rot,
                        int16_t target_step_left,
                        int16_t target_step_right)
{
    struct range_data range;    // Range finder value
    struct mouse_data_speed speed;    // Speed value
    // TODO: Prevent underflow/overflow.
    struct mouse_data_step total_step = { // Accumulated step (distance) values
        .left = MEASURE_ENCODER_DEFAULT,
        .right = MEASURE_ENCODER_DEFAULT,
    };
    struct mouse_data_step target_step;   // Target step (distance) values
    struct wheel_status target_wheel_dir; // Target wheel direction

    // Load PID profiles
    pid_set_pid(&pid->tran, pid_tran);
    pid_set_pid(&pid->rot, pid_rot);

    // Set target speed values
    pid_input_setpoint(&pid->tran, target_speed_tran);
    pid_input_setpoint(&pid->rot, target_speed_rot);

    // Set target steps (or travel distance)
    target_step.left = MEASURE_ENCODER_DEFAULT + target_step_left;
    target_step.right = MEASURE_ENCODER_DEFAULT + target_step_right;

    // Set wheel directions
    target_wheel_dir.left = WHEEL_FORWARD;
    target_wheel_dir.right = WHEEL_FORWARD;

    while (1) {
        control_loop_thread_wait_1ms();
        update_steps_and_speed(&total_step, &speed);
        update_range(&range);

        // if it is too close stop
        if ((range.front > MEASURE_RANGE_F_NEAR_DETECT) ||
            (range.front_right > MEASURE_RANGE_F_NEAR_DETECT)) {
            stop_motor(pid);
            break;
        }

        /* compute PID */
        // calculate errorT
        int32_t feedback_T = (speed.left + speed.right) / 2;
        // calculate errorR
        int32_t feedback_R;
        if((range.left > (MEASURE_RANGE_L_MIN_DETECT))
                            && (range.right > MEASURE_RANGE_R_MIN_DETECT) ) {
            // If both range are within wall detecting distance,
            // Use range sensor to get rotational error
            feedback_R = (range.right - range.left - MEASURE_RANGE_R_OFFSET) / 10;
        } else if (range.left > (MEASURE_RANGE_L_MIN_DETECT + 50)) {
            // If only left side is within range
            // use the middle value of the right range
            feedback_R = (MEASURE_RANGE_R_M_DETECT - range.left) / 10;
        } else if (range.right > (MEASURE_RANGE_R_MIN_DETECT + 50)) {
            // If only right side is within range
            // use the middle value of the left range
            feedback_R = (range.right - MEASURE_RANGE_L_M_DETECT) / 10;
        } else {
            // in open space, use rotary encoder
            feedback_R = speed.diff;
        }
        // Calculate PID outputs
        int32_t outputT = pid_compute(&pid->tran, feedback_T);
        int32_t outputR = pid_compute(&pid->rot, feedback_R);

        // Apply to the motor
        motor_speed_permyriad(CH_LEFT, outputT + outputR);
        motor_speed_permyriad(CH_RIGHT, outputT - outputR);
        motor_start(CH_BOTH);

        logger_log(pid, &range, &total_step, &speed, outputT, outputR);

        if (check_escape_condition(pid, &total_step, &target_step,
                                   &target_wheel_dir)) {
            break;
        }
    }
}

void loop_pivot (struct mouse_data_pid *pid,
                 pid_value_t *pid_tran,
                 pid_value_t *pid_rot,
                 int32_t target_speed_tran,
                 int32_t target_speed_rot,
                 int16_t target_step_left,
                 int16_t target_step_right)
{
    struct mouse_data_speed speed;    // Speed value
    // TODO: Prevent underflow/overflow.
    struct mouse_data_step total_step = { // Accumulated step (distance) values
        .left = MEASURE_ENCODER_DEFAULT,
        .right = MEASURE_ENCODER_DEFAULT,
    };
    struct mouse_data_step target_step;   // Target step (distance) values
    struct wheel_status target_wheel_dir; // Target wheel direction

    // Load PID profiles
    pid_set_pid(&pid->tran, pid_tran);
    pid_set_pid(&pid->rot, pid_rot);

    // Reset PID, or reset accumulated (integrated) speed error
    pid_reset(&pid->tran);
    pid_reset(&pid->rot);

    // Set target speed values
    pid_input_setpoint(&pid->tran, target_speed_tran);
    pid_input_setpoint(&pid->rot, target_speed_rot);

    // Set target steps (or travel distance)
    target_step.left = MEASURE_ENCODER_DEFAULT + target_step_left;
    target_step.right = MEASURE_ENCODER_DEFAULT + target_step_right;

    // Set wheel directions
    target_wheel_dir.left = (target_step_left > 0) ? WHEEL_FORWARD : WHEEL_BACKWARD;
    target_wheel_dir.right = (target_step_right > 0) ? WHEEL_FORWARD : WHEEL_BACKWARD;

    while (1) {
        control_loop_thread_wait_1ms();
        update_steps_and_speed(&total_step, &speed);

        /* Calculate PID */
        // calculate errorT
        int32_t feedback_T = (speed.left + speed.right) / 2;
        // calculate errorR
        int32_t feedback_R = speed.diff;
        // Calculate PID outputs
        int32_t outputT = pid_compute(&pid->tran, feedback_T);
        int32_t outputR = pid_compute(&pid->rot, feedback_R);

        // Apply to the motor
        motor_speed_permyriad(CH_LEFT, outputT + outputR);
        motor_speed_permyriad(CH_RIGHT, outputT - outputR);
        motor_start(CH_BOTH);

        logger_log(pid, NULL, &total_step, &speed, outputT, outputR);

        if (check_escape_condition(pid, &total_step, &target_step,
                                    &target_wheel_dir)) {
            stop_motor(pid);
            break;
        }
    }
}

void loop_smooth_trun (struct mouse_data_pid *pid,
                       pid_value_t *pid_tran,
                       pid_value_t *pid_rot,
                       int32_t target_speed_tran,
                       int32_t target_speed_rot,
                       int16_t target_step_left,
                       int16_t target_step_right)
{
    struct mouse_data_speed speed;    // Speed value
    // TODO: Prevent underflow/overflow.
    struct mouse_data_step total_step = { // Accumulated step (distance) values
        .left = MEASURE_ENCODER_DEFAULT,
        .right = MEASURE_ENCODER_DEFAULT,
    };
    struct mouse_data_step target_step;   // Target step (distance) values
    struct wheel_status target_wheel_dir; // Target wheel direction

    // Load PID profiles
    pid_set_pid(&pid->tran, pid_tran);
    pid_set_pid(&pid->rot, pid_rot);

    // Reset PID, or reset accumulated (integrated) speed error
    pid_reset(&pid->tran);
    pid_reset(&pid->rot);

    // Set target speed values
    pid_input_setpoint(&pid->tran, target_speed_tran);
    pid_input_setpoint(&pid->rot, target_speed_rot);

    // Set target steps (or travel distance)
    target_step.left = MEASURE_ENCODER_DEFAULT + target_step_left;
    target_step.right = MEASURE_ENCODER_DEFAULT + target_step_right;

    // Set wheel directions
    target_wheel_dir.left = WHEEL_FORWARD;
    target_wheel_dir.right = WHEEL_FORWARD;

    while (1) {
        control_loop_thread_wait_1ms();
        update_steps_and_speed(&total_step, &speed);

        /* Calculate PID */
        // calculate errorT
        int32_t feedback_T = (speed.left + speed.right) / 2;
        // calculate errorR
        int32_t feedback_R = speed.diff;
        // Calculate PID outputs
        int32_t outputT = pid_compute(&pid->tran, feedback_T);
        int32_t outputR = pid_compute(&pid->rot, feedback_R);

        // Apply to the motor
        motor_speed_permyriad(CH_LEFT, outputT + outputR);
        motor_speed_permyriad(CH_RIGHT, outputT - outputR);
        motor_start(CH_BOTH);

        logger_log(pid, NULL, &total_step, &speed, outputT, outputR);

        if (check_escape_condition(pid, &total_step, &target_step,
                                    &target_wheel_dir)) {
            break;
        }
    }
}

/*******************************************************************************
 * Common function definitions (etc.)
 ******************************************************************************/
void update_steps_and_speed (struct mouse_data_step *total_step, struct mouse_data_speed *speed)
{
    struct encoder_data step;
    // Update steps from rotary encoders
    encoder_get(&step, ENCODER_CH_BOTH);
    encoder_reset(ENCODER_CH_BOTH); // Then reset
    // update speed
    speed->left = (step.left - MEASURE_ENCODER_DEFAULT);// * CONFIG_LEN_PER_CNT;
    speed->right = (step.right - MEASURE_ENCODER_DEFAULT);// * CONFIG_LEN_PER_CNT;
    speed->diff = speed->left - speed->right;
    // Accumulate steps
    total_step->left += speed->left;
    total_step->right += speed->right;
}

void update_range (struct range_data *range)
{
    range_get(range, RANGE_CH_ALL);

    if (range->left > MEASURE_RANGE_L_MAX_DETECT) {
        range->left = MEASURE_RANGE_L_MAX_DETECT;
    }

    if (range->right > MEASURE_RANGE_R_MAX_DETECT) {
        range->right = MEASURE_RANGE_R_MAX_DETECT;
    }
}

void stop_motor (struct mouse_data_pid *pid)
{
    motor_speed_percent(CH_BOTH, 0);
    motor_start(CH_BOTH);
    pid_reset(&pid->tran);
    pid_reset(&pid->rot);
}

int check_escape_condition (struct mouse_data_pid *pid,
                            struct mouse_data_step *total_step,
                            struct mouse_data_step *target_step,
                            struct wheel_status *target_wheel_dir)
{
    /* Escape condition */
    // update both wheels
    // left
    if (target_wheel_dir->left == WHEEL_FORWARD) {
        if (total_step->left > target_step->left) {
            target_wheel_dir->left = WHEEL_DISABLED;
        }
    } else if (target_wheel_dir->left == WHEEL_BACKWARD) {
        if (total_step->left < target_step->left) {
            target_wheel_dir->left = WHEEL_DISABLED;
        }
    }

    // right
    if (target_wheel_dir->right == WHEEL_FORWARD) {
        if (total_step->right > target_step->right) {
            target_wheel_dir->right = WHEEL_DISABLED;
        }
    } else if (target_wheel_dir->right == WHEEL_BACKWARD) {
        if (total_step->right < target_step->right) {
            target_wheel_dir->right = WHEEL_DISABLED;
        }
    }

    // if both wheels are completed then a command is complete
    if ((target_wheel_dir->left == WHEEL_DISABLED) &&
        (target_wheel_dir->right == WHEEL_DISABLED)) {
        // Escape loop
        return 1;
    }
    return 0;
}
