/*
 * kb_VL6180X_range_finder.h
 *
 *  Created on: Nov 3, 2016
 *      Author: Bumsik Kim
 */

#ifndef MODULE_KB_VL6180X_RANGE_FINDER_H_
#define MODULE_KB_VL6180X_RANGE_FINDER_H_

#include <kb_common_source.h>
#include "kb_module_config.h"

#ifndef VL6180X_I2C
    #define VL6180X_I2C			I2C1
    #define VL6180X_SDA_PORT	GPIOB
    #define VL6180X_SDA_PIN		GPIO_PIN_9
    #define VL6180X_SCL_PORT	GPIOB
    #define VL6180X_SCL_PIN		GPIO_PIN_8
#endif

#ifdef __cplusplus
extern "C" {
#endif

int		vl6180x_init(void);
int32_t  vl6180x_range_mm(void);
uint32_t vl6180x_als_lux(void);

void  vl6180x_start_freerun(void);
int32_t  vl6180x_freerun_range_mm(void);

// TODO: vl6180x_range_als_alt_poll.c
// void Sample_AlternateRangeAls(void);

// TODO: vl6180x_range_interrupt.c
// void Sample_InterrupHandler(void);
// void Sample_Interrupt(void);

// TODO: vl6180x_xtalk_comp.c
// #define N_MEASURE_AVG   20
// int Sample_Init(VL6180xDev_t myDev);
// int Sample_XTalkRunCalibration(VL6180xDev_t myDev);
// void Sample_XTalkCalibrate(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULE_KB_VL6180X_RANGE_FINDER_H_ */
