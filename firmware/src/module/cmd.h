/*
 * motion_controller.h
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#ifndef MOTION_CONTROLLER_H_
#define MOTION_CONTROLLER_H_

#include "thread_control_loop.h"
#include "pid.h"

#ifdef __cplusplus
extern "C" {
#endif

enum cmd_type {
    CMD_NOTHING                 = 0,
    // high level commands
    CMD_MOVE_FROM_BACK_TO_SART_CENTER     = 1,    //< move forward from the back of the cell to the center of the cell
    CMD_MOVE_FORWARD_ONE_CELL   = 2,    //< move forward 1 cell
    CMD_MOVE_FORWARD_HALF_CELL  = 3,    //< move forward 1/2 cell
    CMD_PIVOT_LEFT_90_DEGREE    = 4,    //< turn left in-place
    CMD_PIVOT_RIGHT_90_DEGREE   = 5,    //< turn right in-place
    CMD_TURN_LEFT_SMOOTH        = 6,    //< turn left smooth
    CMD_TURN_RIGHT_SMOOTH       = 7,    //< turn right smooth
    // low commands
    CMD_LOW_SET_PID_AND_GO          = 8,    //< Set PID and start controlling the motors
    CMD_LOW_RESET_PID_AND_STOP  = 9,    //< Reset current accumulated I-value of PID and then stop the motors
};

struct cmd_pid {
    int32_t setpoint_trans;
    int32_t setpoint_rot;
    // int32_t step_left;  //< if it is zero, not target step
    // int32_t step_right; //< if it is zero, not target step
};

struct cmd_events {
    void (*on_start)(void); //< on start callback pointer
    void (*on_completed)(void); //< on completed callback pointer
};

struct cmd_queue_element {
    enum cmd_type type;
    struct cmd_pid pid;
    // uint8_t stop_after;         //< Stop after operation
    // uint8_t get_walls_after;    //< Use range sensors to check walls
};

void cmd_init(void);
void cmd_low_pid_and_go(struct cmd_pid *cmd, struct cmd_events *events);
void cmd_low_pid_reset_and_stop(struct cmd_events *events);

void cmd_polling(enum cmd_type type);
// void cmd_async()

// void motion_queue(motion_cmd_t *commend);

#ifdef __cplusplus
}
#endif

#endif /* MOTION_CONTROLLER_H_ */
