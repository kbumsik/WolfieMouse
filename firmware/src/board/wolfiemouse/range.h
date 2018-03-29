/*
 * range.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Bumsik Kim
 */

#ifndef SRC_BOARD_WOLFIEMOUSE_RANGE_H_
#define SRC_BOARD_WOLFIEMOUSE_RANGE_H_

#include "common_source.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RANGE_MAX 4095

struct range_data {
    uint16_t left;
    uint16_t right;
    uint16_t front;
    // uint16_t front_left;
    uint16_t front_right;
};

enum range_ch {
    RANGE_CH_LEFT,
    RANGE_CH_RIGHT,
    RANGE_CH_LR,
    // RANGE_CH_FRONT_LEFT,
    // RANGE_CH_FRONT_RIGHT,
    // RANGE_CH_FRONT_LR,
    RANGE_CH_FRONT,
    RANGE_CH_ALL
};

void range_init(void);
void range_get(struct range_data *out, enum range_ch ch);

#ifdef __cplusplus
}
#endif


#endif /* ENCODER_H_ */
