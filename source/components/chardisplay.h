/*
 * chardisplay.h
 * HCMS-290x LED 4 Character Display driver.
 *  Created on: Sep 22, 2016
 *      Author: Bumsik Kim
 */

#ifndef COMPONENTS_CHARDISPLAY_H_
#define COMPONENTS_CHARDISPLAY_H_

/*
 * Pin configureation
 * DIS_RS		:PC4	When 1: Select control register, When 0: Dot register
 * 						Value of RS is latched on the falling edge of ~CE input
 * ~DIS_RESET	:PC5
 * ~DIS_CE		:PA4	When 0: enable writing, When 1: Finish Writing
 * DIS_MOSI		:PA7
 * DIS_SCK		:PA5
 */
#define DISPLAY_RS_PORT 	GPIOC
#define DISPLAY_RS_PIN		GPIO_PIN_4
#define DISPLAY_RESET_PORT 	GPIOC
#define DISPLAY_RESET_PIN	GPIO_PIN_5
#define DISPLAY_CE_PORT 	GPIOA
#define DISPLAY_CE_PIN		GPIO_PIN_4
#define DISPLAY_MOSI_PORT 	GPIOA
#define DISPLAY_MOSI_PIN	GPIO_PIN_7
#define DISPLAY_SCK_PORT 	GPIOA
#define DISPLAY_SCK_PIN		GPIO_PIN_5

#ifdef __cplusplus
extern "C"{
#endif

void display_init(void);
void display_matrix(const char *s);
void display_clear(void);
void display_float(float f);
void display_int(int i);
void display_matrix_scroll(char* str);
void display_err(int err);

#ifdef __cplusplus
}
#endif


#endif /* COMPONENTS_CHARDISPLAY_H_ */
