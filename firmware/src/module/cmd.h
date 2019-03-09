/*
 * motion_controller.h
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#ifndef MOTION_CONTROLLER_H_
#define MOTION_CONTROLLER_H_

#include "pid.h"
#include "range.h"

#ifdef __cplusplus
extern "C" {
#endif

enum cmd_type {
    CMD_NOTHING                 = 0,
    // high level commands
    CMD_MOVE_FROM_BACK_TO_SART_CENTER,      //< move forward from the back of the cell to the center of the cell
    CMD_MOVE_FORWARD_ONE_CELL,              //< move forward 1 cell
    CMD_MOVE_FORWARD_HALF_CELL,             //< move forward 1/2 cell
    CMD_MOVE_UNTIL_FRONT_WALL,              //< move forward until the mouse faces a wall
    CMD_MOVE_UNTIL_FRONT_WALL_SCANING,      //< move forward until the mouse faces a wall, while sending range data for each wall the mouse passes.
    CMD_PIVOT_LEFT_90_DEGREE,               //< turn left in-place
    CMD_PIVOT_RIGHT_90_DEGREE,              //< turn right in-place
    CMD_TURN_LEFT_SMOOTH,                   //< turn left smooth
    CMD_TURN_RIGHT_SMOOTH,                  //< turn right smooth
    // Non-blocking commands
    CMD_NONBLOCK_SET_PID_AND_GO,    //< Set PID and start controlling the motors
    CMD_NONBLOCK_RESET_PID_AND_STOP,    //< Reset current accumulated I-value of PID and then stop the motors
    // Sensor commands
    CMD_SENSOR_GET_RANGE,   //< Get range sensor value from CMD_RESP_RANGE
};

enum cmd_response_type {
    CMD_RESP_DONE               = 0,    // Command complete
    CMD_RESP_RANGE              = 1     // Range finder result
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

struct cmd_command {
    enum cmd_type type;
    struct cmd_pid pid;
};

struct cmd_response {
    enum cmd_response_type type;
    struct range_data range;
};

/**
 * @brief Init cmd
 */
void cmd_init(void);

/**
 * @brief Send a command to a control loop thread and wait.
 * @param cmd_type          type of a command to send.
 * @param escape_callback   escape_callback is called when a response is received.
 *                          A command will receive more than one response
 *                          depending on types of the command. The callback MUST
 *                          return non-zero when you don't want to wait anymore.
 *                          It is HIGHLY recommanded to return non-zero when
 *                          response.type is CMD_RESP_DONE.
 */
void cmd_polling(enum cmd_type type, int (*escape_callback)(struct cmd_response));

// void cmd_async()
// void motion_queue(motion_cmd_t *commend);

#ifdef __cplusplus
}
#endif

#endif /* MOTION_CONTROLLER_H_ */
