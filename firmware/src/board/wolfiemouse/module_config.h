/*
 * module_config.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#ifndef BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_
#define BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_

// Push button settings
#define B1_PORT 		PORTB
#define B1_PIN 			PIN_14

#define B2_PORT 		PORTB
#define B2_PIN 			PIN_15

// LED settings
#define LED1_PORT	PORTB
#define LED1_PIN	PIN_1

#define LED2_PORT	PORTB
#define LED2_PIN	PIN_0

#define LED3_PORT	PORTB
#define LED3_PIN	PIN_2

#define LED4_PORT	PORTB
#define LED4_PIN	PIN_10
#define LED5_PORT	PORTB
#define LED5_PIN	PIN_12

#define LED6_PORT	PORTB
#define LED6_PIN	PIN_13

// Terminal settings
#define TERMINAL_UART			USART2
#define TERMINAL_BAUD_RATE		921600 // 9600 or 115200 or 921600 or 1152000 or 1382400 max 2000000

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


// Motor
#define MOTOR_LEFT_PWM_PORT PORTA
#define MOTOR_LEFT_PWM_PIN  PIN_8

#define MOTOR_RIGHT_PWM_PORT PORTA
#define MOTOR_RIGHT_PWM_PIN  PIN_11

// Encoder
// Left encoder
#define ENCODER_L_TIMER     KB_ENCODER_TIM8
#define ENCODER_L_CHA_PORT  PORTC
#define ENCODER_L_CHA_PIN   PIN_6

#define ENCODER_L_CHB_PORT  PORTC
#define ENCODER_L_CHB_PIN   PIN_7
#define ENCODER_L_PIN_PULLUP NOPULL
// Right encoder
#define ENCODER_R_TIMER     KB_ENCODER_TIM3
#define ENCODER_R_CHA_PORT  PORTB
#define ENCODER_R_CHA_PIN   PIN_4

#define ENCODER_R_CHB_PORT  PORTB
#define ENCODER_R_CHB_PIN   PIN_5
#define ENCODER_R_PIN_PULLUP NOPULL

/* Rane finders */
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
#define EMITTER_L_PORT      GPIOA
#define EMITTER_L_PIN       GPIO_PIN_0
#define EMITTER_R_PORT      GPIOC
#define EMITTER_R_PIN       GPIO_PIN_14
#define EMITTER_FL_PORT     GPIOA
#define EMITTER_FL_PIN      GPIO_PIN_1


#endif /* BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_ */
