/*
 * MODULE_HCMS-290X_DISPLAY_H_
 * HCMS-290x LED 4 Character Display driver.
 *  Created on: Sep 22, 2016
 *      Author: Bumsik Kim
 */

#ifndef _MODULE_HCMS_290X_DISPLAY_H_
#define _MODULE_HCMS_290X_DISPLAY_H_

/* Includes */
#include "kb_common_header.h"
#include "kb_module_config.h"

/*
 * Pin configureation
 * DIS_RS		When 1: Select control register, When 0: Dot register
 * 					Value of RS is latched on the falling edge of ~CE input
 * ~DIS_RESET
 * ~DIS_CE		When 0: enable writing, When 1: Finish Writing
 * DIS_MOSI
 * DIS_SCK
 */
#ifndef HCMS_290X_SPI
    #define HCMS_290X_SPI			SPI2
    #define HCMS_290X_RS_PORT		GPIOC
    #define HCMS_290X_RS_PIN		GPIO_PIN_4
    #define HCMS_290X_RESET_PORT 	GPIOC
    #define HCMS_290X_RESET_PIN		GPIO_PIN_5
    #define HCMS_290X_CE_PORT		GPIOA
    #define HCMS_290X_CE_PIN		GPIO_PIN_4
    #define HCMS_290X_MOSI_PORT 	GPIOC
    #define HCMS_290X_MOSI_PIN		GPIO_PIN_1
    #define HCMS_290X_SCK_PORT		GPIOB
    #define HCMS_290X_SCK_PIN		GPIO_PIN_10
#endif

#ifdef __cplusplus
extern "C"{
#endif

void hcms_290x_init(void);
void hcms_290x_matrix(char *s);
void hcms_290x_clear(void);
void hcms_290x_float(float f);
void hcms_290x_int(int i);
void hcms_290x_matrix_scroll(char* str);
void hcms_290x_err(int err);

#ifdef __cplusplus
}
#endif


#endif /* _MODULE_HCMS_290X_DISPLAY_H_ */
