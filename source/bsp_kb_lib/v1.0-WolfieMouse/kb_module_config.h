/*
 * module_config.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#ifndef BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_
#define BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_

// Push button settings
#define B1_PORT 		PORTA
#define B1_PIN 			PIN_15

#define B2_PORT 		PORTA
#define B2_PIN 			PIN_12

// LED1 settings
#define LED1_PORT	PORTB
#define LED1_PIN	PIN_0

#define LED2_PORT	PORTB
#define LED2_PIN	PIN_1

#define LED3_PORT	PORTC
#define LED3_PIN	PIN_11

#define LED4_PORT	PORTC
#define LED4_PIN	PIN_10

// Terminal settings
#define TERMINAL_UART			USART2
#define TERMINAL_BAUD_RATE		9600 // or 115200

#define TERMINAL_TX_PORT 		GPIOA
#define TERMINAL_TX_PIN 		GPIO_PIN_2

#define TERMINAL_RX_PORT 		GPIOA
#define TERMINAL_RX_PIN 		GPIO_PIN_3

// HCMS_290X settings
#define HCMS_290X_SPI			SPI1

#define HCMS_290X_RS_PORT		PORTC
#define HCMS_290X_RS_PIN		PIN_4

#define HCMS_290X_RESET_PORT 	PORTC
#define HCMS_290X_RESET_PIN		PIN_5

#define HCMS_290X_CE_PORT		PORTA
#define HCMS_290X_CE_PIN		PIN_4

#define HCMS_290X_MOSI_PORT 	PORTA
#define HCMS_290X_MOSI_PIN		PIN_7

#define HCMS_290X_SCK_PORT		PORTA
#define HCMS_290X_SCK_PIN		PIN_5

// TCA9545A
#define TCA9545A_I2C		I2C1

#define TCA9545A_SDA_PORT	PORTB
#define TCA9545A_SDA_PIN	PIN_9

#define TCA9545A_SCL_PORT	PORTB
#define TCA9545A_SCL_PIN	PIN_8

#define TCA9545A_RESET_PORT	PORTC
#define TCA9545A_RESET_PIN	PIN_13

// VL6180X
#define VL6180X_I2C			I2C1

#define VL6180X_SDA_PORT	GPIOB
#define VL6180X_SDA_PIN		GPIO_PIN_9

#define VL6180X_SCL_PORT	GPIOB
#define VL6180X_SCL_PIN		GPIO_PIN_8

// Motor
#define MOTOR_LEFT_PWM_PORT PORTA
#define MOTOR_LEFT_PWM_PIN  PIN_8

#define MOTOR_RIGHT_PWM_PORT PORTA
#define MOTOR_RIGHT_PWM_PIN  PIN_11

// ADC for range sensors
#define RECV_ADC            KB_ADC1
#define RECV_FR_PORT        GPIOC
#define RECV_FR_PIN         GPIO_PIN_0
#define RECV_R_PORT         GPIOC
#define RECV_R_PIN          GPIO_PIN_1
#define RECV_L_PORT         GPIOC
#define RECV_L_PIN          GPIO_PIN_2
#define RECV_FL_PORT        GPIOC
#define RECV_FL_PIN         GPIO_PIN_3

// Range sensor emitters
// right front : PC13
// sides : PA0
// left front : PA1
#define EMITTER_FR_PORT     GPIOC
#define EMITTER_FR_PIN      GPIO_PIN_13
#define EMITTER_SIDES_PORT  GPIOA
#define EMITTER_SIDES_PIN   GPIO_PIN_0
#define EMITTER_FL_PORT     GPIOA
#define EMITTER_FL_PIN      GPIO_PIN_1


#endif /* BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_ */
