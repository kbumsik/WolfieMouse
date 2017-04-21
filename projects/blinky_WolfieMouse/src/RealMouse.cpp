/*
 * RealMouse.cpp
 *
 *  Created on: Apr 7, 2017
 *      Author: kbumsik
 */

#include "RealMouse.hpp"
#include "system_control.h"
#include "config_measurements.h"

#include "encoder.h"
#include "pid.h"
/*******************************************************************************
 * Global Variables
 ******************************************************************************/
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
 * Private functions
 ******************************************************************************/
void moveForward(uint32_t steps);
void rotateInPlace(uint32_t steps, int cw);

/*******************************************************************************
 * Constructor
 ******************************************************************************/
RealMouse::RealMouse()
{
    // The initializer does it all
}

/*******************************************************************************
 * Public Methods
 ******************************************************************************/
/* Overriding FinderInterface methods */
Wall RealMouse::examineWall(int row, int col, Direction wallDir, PositionController &mousePos)
{
    Direction mouseDir = mousePos.getCurrentDir();
    volatile uint16_t sensorValue;
    if (wallDir == mouseDir) {
        // Use front sensor
        uint16_t sensorValue = range_F;
        return (range_F > MEASURE_RANGE_F_DETECT)?wall:empty;
    } else if (wallDir == (++mouseDir)) {
        // Use left sensor
        return (range_L > MEASURE_RANGE_L_DETECT)?wall:empty;
    } else if ((++wallDir) == (--mouseDir)) {
        return (range_R > MEASURE_RANGE_R_DETECT)?wall:empty;
    }
    return wallError;
}

/* Overriding MoverInterface methods */
void RealMouse::moveTo(int row, int col, Direction destDir, PositionController &mousePos)
{
    // Just move it by one cell
    moveForward(MEASURE_STEPS_PER_CELL);
    return;
}

void RealMouse::rotateTo(Direction destDir, PositionController &mousePos)
{
    Direction mouseDir = mousePos.getCurrentDir();
    if (destDir == mouseDir) {
        // The direction is the same
        return;
    } else if (destDir == (++mouseDir)) {
        // Turn left
        rotateInPlace(MEASURE_STEPS_90DEG_CCW, 0);
    } else if ((++destDir) == (--mouseDir)) {
        // Turn right
        rotateInPlace(MEASURE_STEPS_90DEG_CW, 1);
    } else {
        // turn 180 degree
        // Turn left
        rotateInPlace(MEASURE_STEPS_90DEG_CCW * 2, 0);
    }
}

void moveForward(uint32_t steps)
{
    // Reset encoder
    system_reset_encoder();

    /* Motor test running */
    system_enable_range_finder();

    pid_reset(&g_pid_T);
    pid_reset(&g_pid_R);
    pid_input_setpoint(&g_pid_T, 25);
    pid_input_setpoint(&g_pid_R, 0);

    system_start_driving();


    // wait for distance of one cell
    for(; encoder_right_count() <
                (steps + MEASURE_ENCODER_DEFAULT);) {
    }

    /* Motor test running done */
    system_stop_driving();
    system_disable_range_finder();
    pid_reset(&g_pid_T);
    pid_reset(&g_pid_R);
}

void rotateInPlace(uint32_t steps, int cw)
{
    // Reset encoder
    system_reset_encoder();

    /* Motor test running */
    system_disable_range_finder();
    pid_reset(&g_pid_T);
    pid_reset(&g_pid_R);
    switch (cw) {
    case 0:
        pid_input_setpoint(&g_pid_T, 0);
        pid_input_setpoint(&g_pid_R, -60);
        system_start_driving();

        // wait for distance of one cell
        for(; encoder_right_count() <
                    (MEASURE_ENCODER_DEFAULT + MEASURE_STEPS_90DEG_CCW);) {
        }

        break;
    default:    // Clockwise
        pid_input_setpoint(&g_pid_T, 0);
        pid_input_setpoint(&g_pid_R, 60);
        system_start_driving();

        // wait for distance of one cell
        for(; encoder_right_count() >
                    (MEASURE_ENCODER_DEFAULT - steps);) {
        }
        break;
    }
    /* Motor test running done */
    system_stop_driving();
    pid_reset(&g_pid_T);
    pid_reset(&g_pid_R);
}
