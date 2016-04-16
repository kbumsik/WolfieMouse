/*
 * pidMotor.h
 *
 *  Created on: Apr 16, 2016
 *      Author: Bumsik Kim
 */

#ifndef PIDMOTOR_H_
#define PIDMOTOR_H_

#include "Motor.h"

#ifdef __cplusplus
extern "C" {
#endif

void pidMotorInit(void);
void pidMotorStart(eMotorChannel_t channel);
void pidMotorStop(eMotorChannel_t channel);
void pidMotorSetSpeed(eMotorChannel_t channel, int speed);
void pidMotorUpdateToCenter(void);

void pidMotorMoveFor1Cell(int speed);
void pidMotorRotateFor90Degree(int speed);
void pidMotorMoveForHalfCell(int speed);
void pidMotorRotateFor180Degree(int speed);

#ifdef __cplusplus
}
#endif

#endif /* PIDMOTOR_H_ */
