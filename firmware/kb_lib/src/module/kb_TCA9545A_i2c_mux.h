/*
 * kb_TCA9545A_i2c_mux.h
 *
 *  Created on: Oct 30, 2016
 *      Author: Bumsik Kim
 */

#ifndef MODULE_KB_TCA9545A_I2C_MUX_H_
#define MODULE_KB_TCA9545A_I2C_MUX_H_


#include "kb_common_header.h"
#include "kb_module_config.h"

#ifndef TCA9545A_I2C
    #define TCA9545A_I2C		I2C1
    #define TCA9545A_SDA_PORT	GPIOB
    #define TCA9545A_SDA_PIN	GPIO_PIN_9
    #define TCA9545A_SCL_PORT	GPIOB
    #define TCA9545A_SCL_PIN	GPIO_PIN_8
    #define TCA9545A_RESET_PORT	GPIOC
    #define TCA9545A_RESET_PIN	GPIO_PIN_13
#endif

#define TCA9545A_CH_0	0x01U
#define TCA9545A_CH_1	0x02U
#define TCA9545A_CH_2	0x04U
#define TCA9545A_CH_3	0x08U

#ifdef __cplusplus
extern "C"{
#endif

int tca9545a_init(void);
int tca9545a_select_ch(uint8_t ch);
uint8_t tca9545a_current_ch(void);
uint8_t tca9545a_current_it(void);
uint8_t tca9545a_clear_it(uint8_t ch);

#ifdef __cplusplus
}
#endif


#endif /* MODULE_KB_TCA9545A_I2C_MUX_H_ */
