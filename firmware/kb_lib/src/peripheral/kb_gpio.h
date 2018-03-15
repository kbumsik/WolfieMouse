/*
 * kb_gpio.h
 *
 *  Created on: Oct 25, 2016
 *      Author: Bumsik Kim
 */

#ifndef PERIPHERAL_KB_GPIO_H_
#define PERIPHERAL_KB_GPIO_H_

#include "kb_common_source.h"

#if defined(STM32)
    // typedefs
    /* TODO: Think of a good design for GPIO */
    typedef void*   kb_gpio_port_t;
    typedef uint16_t    kb_gpio_pin_t;
    typedef GPIO_InitTypeDef 	kb_gpio_init_t;
    typedef GPIO_PinState 		kb_gpio_state_t;
    typedef	uint32_t			kb_gpio_pull_t;
    typedef enum {
        RISING_EDGE,
        FALLING_EDGE,
        BOTH_EDGE
    }kb_gpio_edge_t;
    // Pull state
    #define NOPULL		GPIO_NOPULL
    #define PULLUP		GPIO_PULLUP
    #define PULLDOWN	GPIO_PULLDOWN
    // Ports
    #define PORTA	GPIOA
    #define PORTB	GPIOB
    #define PORTC	GPIOC
    #define PORTD	GPIOD
    #define PORTE	GPIOE
    #define PORTF	GPIOF
    #define PORTG	GPIOG
    #define PORTH	GPIOH
    // Pins
    #define PIN_0		GPIO_PIN_0
    #define PIN_1		GPIO_PIN_1
    #define PIN_2		GPIO_PIN_2
    #define PIN_3		GPIO_PIN_3
    #define PIN_4		GPIO_PIN_4
    #define PIN_5		GPIO_PIN_5
    #define PIN_6		GPIO_PIN_6
    #define PIN_7		GPIO_PIN_7
    #define PIN_8		GPIO_PIN_8
    #define PIN_9		GPIO_PIN_9
    #define PIN_10		GPIO_PIN_10
    #define PIN_11		GPIO_PIN_11
    #define PIN_12		GPIO_PIN_12
    #define PIN_13		GPIO_PIN_13
    #define PIN_14		GPIO_PIN_14
    #define PIN_15		GPIO_PIN_15
    #define PIN_All		GPIO_PIN_All
#else
    #error "Please define device driver (e.g. USE_HAL_DRIVER)\n"
#endif


#ifdef __cplusplus
extern "C"{
#endif

void kb_gpio_init(kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_init_t *gpio_init);
kb_gpio_state_t kb_gpio_read(kb_gpio_port_t port, kb_gpio_pin_t pin);
void kb_gpio_set(kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_state_t state);
void kb_gpio_toggle(kb_gpio_port_t port, kb_gpio_pin_t pin);

void kb_gpio_enable_clk(kb_gpio_port_t port);

int kb_gpio_isr_register(kb_gpio_port_t port, kb_gpio_pin_t pin, void (*callback)(void));
int kb_gpio_isr_deregister(kb_gpio_port_t port, kb_gpio_pin_t pin);
int kb_gpio_isr_enable(kb_gpio_port_t port, kb_gpio_pin_t pin, kb_gpio_init_t *gpio_init, kb_gpio_edge_t edge);
int kb_gpio_isr_disable(kb_gpio_port_t port, kb_gpio_pin_t pin);


#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_KB_GPIO_H_ */
