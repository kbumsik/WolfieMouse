/*
 * RealMouse.cc
 *
 *  Created on: Apr 7, 2017
 *      Author: kbumsik
 */

#include "RealMouse.hpp"
#include "system_control.h"
#include "config_measurements.h"

#include "range.h"
#include "cmd.h"

// For LEDs
#include "module_config.h"
#include "gpio.h"
#include "tick.h"

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

extern volatile struct range_data range;
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
        ret = (range.front > MEASURE_RANGE_F_DETECT)?wall:empty;
    } else if (wallDir == (++mouseDir)) {
        // Use left sensor
        ret = (range.left > MEASURE_RANGE_L_DETECT)?wall:empty;
    } else if ((++wallDir) == (--mouseDir)) {
        ret = (range.right > MEASURE_RANGE_R_DETECT)?wall:empty;
    }
    gpio_set(LED3_PORT, LED3_PIN, GPIO_PIN_RESET);
    return ret;
}

/* Overriding MoverInterface methods */
void RealMouse::moveTo(int row, int col, Direction destDir, PositionController &mousePos)
{
    gpio_set(LED5_PORT, LED5_PIN, GPIO_PIN_SET);
    // Just move it by one cell
    cmd_polling(CMD_F);
    cmd_low_pid_reset_and_stop(NULL);
    delay_ms(500);
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
        cmd_polling(CMD_L);
        cmd_low_pid_reset_and_stop(NULL);
        delay_ms(500);
    } else if ((++destDir) == (--mouseDir)) {
        // Turn right
        cmd_polling(CMD_R);
        cmd_low_pid_reset_and_stop(NULL);
        delay_ms(500);
    } else {
        // turn 180 degree
        // Turn left
        cmd_polling(CMD_L);
        cmd_polling(CMD_L);
        cmd_low_pid_reset_and_stop(NULL);
        delay_ms(500);
    }
    gpio_set(LED4_PORT, LED4_PIN, GPIO_PIN_RESET);
}
