/*
 * motion_controller.h
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#ifndef MOTION_CONTROLLER_H_
#define MOTION_CONTROLLER_H_

#include "pid.h"

#ifdef __cplusplus
extern "C" {
#endif

enum cmd_type {
    CMD_NOTHING,
    // low commands
    CMD_LOW_PID_AND_GO, CMD_LOW_PID_RESET_AND_STOP,
    // high level commands
    CMD_F,      //< move forward 1 cell
    CMD_H_F,    //< move forward 1/2 cell
    CMD_L,      //< turn left in-place
    CMD_R,      //< turn right in-place
    CMD_S_L,    //< turn left smooth
    CMD_S_R,    //< turn right smooth
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
