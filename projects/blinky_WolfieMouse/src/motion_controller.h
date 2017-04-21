/*
 * motion_controller.h
 *
 *  Created on: Apr 8, 2017
 *      Author: kbumsik
 */

#ifndef MOTION_CONTROLLER_H_
#define MOTION_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _motion_commend_t {
    enum {
        straight,
        turn,
        curve
    } type;
    float unit;    //< units differ from types.
                    //<    1 unit = 1 cell (straight)
                    //<           = 90 deg (turn)
                    //<           = 90 deg (curve)
                    //< Polarity of units determine direction.
                    //<     Plus: front, Minus: back (stright)
                    //<     Plus: left, Minus: right (turn, curve)
    void (*on_start)(void); //< on start callback pointer
    void (*on_completed)(void); //< on completed callback pointer
} motion_cmd_t;

void motion_controller_init();
void motion_queue(motion_cmd_t *commend);

#ifdef __cplusplus
}
#endif

#endif /* MOTION_CONTROLLER_H_ */
