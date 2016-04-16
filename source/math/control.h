/*
 * control.h
 *
 *  Created on: Jan 2, 2016
 *      Author: Bumsik Kim
 */

#ifndef CONTROL_H_
#define CONTROL_H_


#include "stm32f4xx_hal.h"

#define xControlMaxLimitter(VALUE,MAX)  (VALUE<MAX?VALUE:MAX)
#define xControlMinLimitter(VALUE,MIN)  (VALUE>MIN?VALUE:MIN)
#define xControlLimitter(VALUE,MIN,MAX) (xControlMinLimitter(xControlMaxLimitter(VALUE,MAX),MIN))

#define xControlMinusAndScale(VALUE,OFFSET,SCALE) (((VALUE)-(OFFSET))*(SCALE))
#define xControlMinusAndScaleReversed(VALUE,OFFSET,SCALE) ((VALUE)*(SCALE)-(OFFSET))
#define xControlPlusAndScale(VALUE,OFFSET,SCALE) (((VALUE)+(OFFSET))*(SCALE))
#define xControlPlusAndScaleReversed(VALUE,OFFSET,SCALE) ((VALUE)*(SCALE)+(OFFSET))



#ifdef __cplusplus
extern "C" {
#endif
  /*working variables*/
  typedef struct{
    uint32_t lastTime;
    float errSum, lastErr;
    float kp, ki, kd;
  } controlPid_t;

float fControlPIDCompute(float setpoint, float input, controlPid_t *obj);

void vControlPIDSetTunings(float Kp, float Ki, float Kd, controlPid_t *obj);

#ifdef __cplusplus
}
#endif
#endif /* CONTROL_H_ */
