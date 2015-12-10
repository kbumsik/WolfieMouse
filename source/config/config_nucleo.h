/*
 * config_nucleo.h
 *
 *  Created on: Dec 10, 2015
 *      Author: Bumsik Kim
 */

#ifndef CONFIG_NUCLEO_H_
#define CONFIG_NUCLEO_H_

#ifdef NUCLEO_F466RE

#ifndef LED_PIN
	#define LED_PIN GPIO_PIN_5
	#define LED_PORT GPIOA
#endif

#ifdef USE_HAL_DRIVER
	#ifdef LED_PIN
		#ifdef LED_PORT
			#define LED_TOGGLE()	HAL_GPIO_TogglePin(LED_PORT, LED_PIN)
			#define LED_ON()		HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET)
			#define LED_OFF()		HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET)
		#endif
	#endif
#endif

#endif

#endif /* CONFIG_NUCLEO_H_ */
