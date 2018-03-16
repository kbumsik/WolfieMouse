/*
 * module_config.h
 *
 *  Created on: Oct 26, 2016
 *      Author: Bumsik Kim
 */

#ifndef BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_
#define BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_

// Push button settings
#if defined(KB_WOLFIEMOUSE)
    #define B1_PORT 		PORTA
    #define B1_PIN 			PIN_15

    #define B2_PORT 		PORTA
    #define B2_PIN 			PIN_12
#elif defined(KB_BLACKWOLF)
    #define B1_PORT 		PORTB
    #define B1_PIN 			PIN_14
    
    #define B2_PORT 		PORTB
    #define B2_PIN 			PIN_15
#else
    #error "Define one of WolfieMouse hardware"
#endif

// LED1 settings
#if defined(KB_WOLFIEMOUSE)
    #define LED1_PORT	PORTB
    #define LED1_PIN	PIN_0

    #define LED2_PORT	PORTB
    #define LED2_PIN	PIN_1

    #define LED3_PORT	PORTC
    #define LED3_PIN	PIN_11

    #define LED4_PORT	PORTC
    #define LED4_PIN	PIN_10
#elif defined(KB_BLACKWOLF)
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
#else
    #error "Define one of WolfieMouse hardware"
#endif

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


// Motor
#define MOTOR_LEFT_PWM_PORT PORTA
#define MOTOR_LEFT_PWM_PIN  PIN_8

#define MOTOR_RIGHT_PWM_PORT PORTA
#define MOTOR_RIGHT_PWM_PIN  PIN_11

// Encoder
#if defined(KB_WOLFIEMOUSE)
    // Left encoder
    #define ENCODER_L_TIMER     TIMER3

    #define ENCODER_L_CHA_PORT  PORTC
    #define ENCODER_L_CHA_PIN   PIN_7
    
    #define ENCODER_L_CHB_PORT  PORTC
    #define ENCODER_L_CHB_PIN   PIN_6

    #define ENCODER_L_PIN_PULLUP PULLUP

    // Right encoder
    #define ENCODER_R_TIMER     TIMER4

    #define ENCODER_R_CHA_PORT  PORTB
    #define ENCODER_R_CHA_PIN   PIN_7
    
    #define ENCODER_R_CHB_PORT  PORTB
    #define ENCODER_R_CHB_PIN   PIN_6

    #define ENCODER_R_PIN_PULLUP PULLUP
#elif defined(KB_BLACKWOLF)
    // Left encoder
    #define ENCODER_L_TIMER     KB_ENCODER_TIM8

    #define ENCODER_L_CHA_PORT  PORTC
    #define ENCODER_L_CHA_PIN   PIN_6
    
    #define ENCODER_L_CHB_PORT  PORTC
    #define ENCODER_L_CHB_PIN   PIN_7

    #define ENCODER_L_PIN_PULLUP PULLUP

    // Right encoder
    #define ENCODER_R_TIMER     KB_ENCODER_TIM3

    #define ENCODER_R_CHA_PORT  PORTB
    #define ENCODER_R_CHA_PIN   PIN_4
    
    #define ENCODER_R_CHB_PORT  PORTB
    #define ENCODER_R_CHB_PIN   PIN_5

    #define ENCODER_R_PIN_PULLUP PULLUP
#endif

/* Rane finders */
#if defined(KB_WOLFIEMOUSE)
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
#elif defined(KB_BLACKWOLF)
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
#endif


#endif /* BSP_WOLFIEMOUSE_KB_MODULE_CONFIG_H_ */
