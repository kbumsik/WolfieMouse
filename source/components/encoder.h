/*
 * encoder.h
 *
 *  Created on: Apr 15, 2016
 *      Author: Bumsik Kim
 */

#ifndef ENCODER_H_
#define ENCODER_H_

void encoder_init(void);
int32_t getLeftEncCount(void);
int32_t getRightEncCount(void);
void resetRightEncCount(void);
void resetLeftEncCount(void);

#endif /* ENCODER_H_ */
