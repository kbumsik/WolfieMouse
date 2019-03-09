#include "control_loop_thread.h"
#include "control_loop_thread_driver.h"
#include "mouse_data.h"
#include "logger_thread.h"

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
 * Function definitions
 ******************************************************************************/
void do_nothing (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void move_from_back_to_start_center (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void move_forward_one_cell (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void move_forward_half_cell (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void pivot_left_90_degree (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void pivot_right_90_degree (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void turn_left_smooth (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void turn_right_smooth (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void set_pid_and_go (struct mouse_data_pid *pid)
{
    // Not used
}

void reset_pid_and_stop (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}

void send_range_data (struct mouse_data_pid *pid)
{
    // Do nothing. Literally.
}
