/*
 * RealMouse.cc
 *
 *  Created on: Apr 7, 2017
 *      Author: kbumsik
 */

#include "RealMouse.hpp"
#include "config_measurements.h"

#include "range.h"
#include "cmd.h"

// For LEDs
#include "module_config.h"
#include "gpio.h"
#include "tick.h"

/*******************************************************************************
 * local Variables
 ******************************************************************************/
static struct range_data range;
static auto update_range = [&range](struct cmd_response resp){
    switch (resp.type) {
        case CMD_RESP_RANGE:
            range = resp.range;
            return 0;
        case CMD_RESP_DONE:
            return 1;
        default:
            return 0;
    }
    return 0;
};

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
    gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_SET);
    Direction mouseDir = mousePos.getCurrentDir();

    Wall ret = wallError;
    if (wallDir == mouseDir) {
        // Use front sensor
        ret = (range.front > (MEASURE_RANGE_F_NEAR_DETECT - 100 ))?wall:empty;
    } else if (wallDir == (++mouseDir)) {
        // Use left sensor
        ret = (range.left > MEASURE_RANGE_L_WALL_DETECT)?wall:empty;
    } else if ((++wallDir) == (--mouseDir)) {
        ret = (range.right > MEASURE_RANGE_R_WALL_DETECT)?wall:empty;
    }
    gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_RESET);
    return ret;
}

/* Overriding MoverInterface methods */
void RealMouse::getReady()
{
    gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_SET);
    // TODO: MOVE HALF!!!!!!!!!!!!!
    // Update sensors
    cmd_polling(CMD_SENSOR_GET_RANGE, update_range);
    gpio_set(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    return;
}

/* Overriding MoverInterface methods */
void RealMouse::moveTo(int row, int col, Direction destDir, PositionController &mousePos)
{
    gpio_set(LED5_PORT, LED5_PIN, GPIO_PIN_SET);
    // Just move it by one cell
    cmd_polling(CMD_MOVE_FORWARD_ONE_CELL, NULL);
    // Update sensors
    cmd_polling(CMD_SENSOR_GET_RANGE, update_range);
    gpio_set(LED5_PORT, LED5_PIN, GPIO_PIN_RESET);
    return;
}

void RealMouse::rotateTo(Direction destDir, PositionController &mousePos)
{
    gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_SET);
    Direction mouseDir = mousePos.getCurrentDir();
    if (destDir == mouseDir) {
        // The direction is the same. Nothing
    } else if (destDir == (++mouseDir)) {
        // Turn left
        cmd_polling(CMD_PIVOT_LEFT_90_DEGREE, NULL);
        cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
        delay_ms(500);
    } else if ((++destDir) == (--mouseDir)) {
        // Turn right
        cmd_polling(CMD_PIVOT_RIGHT_90_DEGREE, NULL);
        cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
        delay_ms(500);
    } else {
        // turn 180 degree
        // Turn left
        cmd_polling(CMD_PIVOT_LEFT_90_DEGREE, NULL);
        cmd_polling(CMD_PIVOT_LEFT_90_DEGREE, NULL);
        // TODO: Calibrate position here!!!!!!!!!!!!!!!!!!!!!!
        cmd_polling(CMD_LOW_RESET_PID_AND_STOP, NULL);
        delay_ms(500);
    }
    // Update sensors
    cmd_polling(CMD_SENSOR_GET_RANGE, update_range);
    gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_RESET);
}
