/*
 * encoder.h
 *
 *  Created on: Nov 6, 2016
 *      Author: Bumsik Kim
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "kb_common_source.h"

#ifdef __cplusplus
extern "C" {
#endif

void encoder_init(void);
int32_t encoder_right_count(void);
int32_t encoder_left_count(void);
void encoder_right_reset(void);
void encoder_left_reset(void);

#ifdef __cplusplus
}
#endif


#endif /* ENCODER_H_ */
