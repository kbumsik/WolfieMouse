/*
 * encoder.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Bumsik Kim
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "common_source.h"

#ifdef __cplusplus
extern "C" {
#endif

struct encoder_data {
    int32_t left;
    int32_t right;
};

enum encoder_ch {
    ENCODER_CH_LEFT,
    ENCODER_CH_RIGHT,
    ENCODER_CH_BOTH
};

void encoder_init(void);
void encoder_get(struct encoder_data *out, enum encoder_ch ch);
void encoder_reset(enum encoder_ch ch);

#ifdef __cplusplus
}
#endif


#endif /* ENCODER_H_ */
