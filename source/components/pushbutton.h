/*
 * pushbutton.h
 *
 *  Created on: Sep 22, 2016
 *      Author: Bumsik Kim
 */

#ifndef COMPONENTS_PUSHBUTTON_H_
#define COMPONENTS_PUSHBUTTON_H_

#define PB1_PIN		GPIO_PIN_15
#define PB1_PORT	GPIOA
#define PB2_PIN		GPIO_PIN_12
#define PB2_PORT	GPIOA


typedef enum{
	pb1, pb2
}pb_t;

#ifdef __cplusplus
extern "C" {
#endif

void pb_init(void);
int pb_read(pb_t pushbutton);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_PUSHBUTTON_H_ */
