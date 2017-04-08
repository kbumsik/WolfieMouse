	/*
 * motor.c
 *
 *  Created on: Nov 6, 2016
 *      Author: Bumsik Kim
 */
#include "motor.h"
#include "kb_timer.h"
#include "kb_gpio.h"

#define motorLEFT_PORT          GPIOC
#define motorLEFT_PIN           GPIO_PIN_9
#define motorRIGHT_PORT         GPIOA
#define motorRIGHT_PIN          GPIO_PIN_10

static inline void left_set_toggle_(void)
{
    kb_gpio_toggle(motorLEFT_PORT, motorLEFT_PIN);
}
static inline void right_set_toggle_(void)
{
    kb_gpio_toggle(motorRIGHT_PORT, motorRIGHT_PIN);
}

static inline void left_set_forward_(void)
{
    kb_gpio_set(motorLEFT_PORT, motorLEFT_PIN, GPIO_PIN_SET);
}
static inline void right_set_forward_(void)
{
#if defined(KB_WOLFIEMOUSE)
    kb_gpio_set(motorRIGHT_PORT, motorRIGHT_PIN, GPIO_PIN_SET);
#else
    kb_gpio_set(motorRIGHT_PORT, motorRIGHT_PIN, GPIO_PIN_RESET);
#endif
}

static inline void left_set_backward_()
{
    kb_gpio_set(motorLEFT_PORT, motorLEFT_PIN, GPIO_PIN_RESET);
}
static inline void right_set_backward_(void)
{
#if defined(KB_WOLFIEMOUSE)
	kb_gpio_set(motorRIGHT_PORT, motorRIGHT_PIN, GPIO_PIN_RESET);
#else
    kb_gpio_set(motorRIGHT_PORT, motorRIGHT_PIN, GPIO_PIN_SET);
#endif
}


/**
 * Function Implementations
 */
kb_status_t motor_init(void)
{
	/* init GPIO */
	/****
	 * Direction GPIOs
	 * * *GPIO Pin Configuration:
	 *     left_PWM: PA8
	 *     left_dir: PC9
	 *     left_fault: PC8
	 *     right_PWM: PA11
	 *     right_dir: PA10
	 *     right_fault: PA9
	 */
    kb_timer_ch_pin(TIMER1, CH_1, PORTA, PIN_8, NOPULL);    //left  PWM
	kb_timer_ch_pin(TIMER1, CH_4, PORTA, PIN_11, NOPULL);	//right PWM


	kb_gpio_init_t gpio_setting = {
			.Pull = NOPULL,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Speed = GPIO_SPEED_HIGH
	};
	kb_gpio_init(PORTA, PIN_10, &gpio_setting);

	gpio_setting.Mode = GPIO_MODE_INPUT;
	kb_gpio_init(PORTA, PIN_9, &gpio_setting);

	gpio_setting.Mode = GPIO_MODE_OUTPUT_PP;
	kb_gpio_init(PORTC, PIN_9, &gpio_setting);

	gpio_setting.Mode = GPIO_MODE_INPUT;
	kb_gpio_init(PORTC, PIN_8, &gpio_setting);

	motor_dir_forward();

	// PWM settings
	kb_pwm_init_t pwm_setting = {	// PWM is at 1kHz
			.clock_frequency = 10000000,
			.period = 10000
	};
	kb_pwm_init(TIMER1, &pwm_setting);

	/* Firstly set speed 0 */
	motor_speed_percent(CH_BOTH, 0);
	return KB_OK;
}


int32_t motor_speed_percent(motor_ch_t channel, int32_t speed)
{
    int go_backword = 0;
    if (speed < 0) {
        go_backword = 1;
        speed *= -1;
    }
	switch (channel) {
	case CH_LEFT:
		/* Set the pulse value for channel 1 */
	    if (go_backword) {
	        left_set_backward_();
	    } else {
	        left_set_forward_();
	    }
		kb_pwm_percent(TIMER1, CH_1, speed);
		break;

	case CH_RIGHT:
		/* Set the pulse value for channel 4 */
        if (go_backword) {
            right_set_backward_();
        } else {
            right_set_forward_();
        }
		kb_pwm_percent(TIMER1, CH_4, speed);
		break;

	case CH_BOTH:
		/* Set the pulse value for all channel */
        if (go_backword) {
            left_set_backward_();
            right_set_backward_();
        } else {
            left_set_forward_();
            right_set_forward_();
        }
		kb_pwm_percent(TIMER1, CH_1, speed);
		kb_pwm_percent(TIMER1, CH_4, speed);
		break;

	default:
		/* Return Error in other cases */
		return 0;
	}

	/* return OK */
	return speed;
}

void motor_dir_forward(void)
{
	right_set_forward_();
	left_set_forward_();
}

void motor_dir_backward(void)
{
	right_set_backward_();
	left_set_backward_();
}

void motor_dir_rotate_right(void)
{
	right_set_backward_();
	left_set_forward_();
}

void motor_dir_rotate_left(void)
{
	right_set_forward_();
	left_set_backward_();
}

kb_status_t motor_start(motor_ch_t channel)
{
	kb_status_t result;
	switch (channel)
	{
	case CH_LEFT:
		/* Start channel 1 */
		result = kb_pwm_start(TIMER1, CH_1);
		if(result != KB_OK)
		{
			return result;
		}
		break;
	case CH_RIGHT:
		/* Start channel 4 */
		result = kb_pwm_start(TIMER1, CH_4);
		if(result != KB_OK)
		{
			return result;
		}
		break;
	case CH_BOTH:
        /* Start channel 1 */
        result = kb_pwm_start(TIMER1, CH_1);
        if(result != KB_OK)
        {
            return result;
        }
        /* Start channel 4 */
        result = kb_pwm_start(TIMER1, CH_4);
        if(result != KB_OK)
        {
            return result;
        }
		break;

	default:
		return KB_ERROR;
	}
	/* Return OK */
	return KB_OK;
}

kb_status_t motor_stop(motor_ch_t channel)
{
	kb_status_t result;
	switch (channel)
	{
	case CH_LEFT:
		/* Start channel 1 */
		result = kb_pwm_stop(TIMER1, CH_1);
		if(result != KB_OK)
		{
			return result;
		}
		break;
	case CH_RIGHT:
		/* Start channel 4 */
		result = kb_pwm_stop(TIMER1, CH_4);
		if(result != KB_OK)
		{
			return result;
		}
		break;
	case CH_BOTH:
		/* Start channel 1 */
		result = kb_pwm_stop(TIMER1, CH_1);
		if(result != KB_OK)
		{
			return result;
		}
		/* Start channel 4 */
		result = kb_pwm_stop(TIMER1, CH_4);
		if(result != KB_OK)
		{
			return result;
		}
		break;

	default:
		return KB_ERROR;
	}
	/* Return OK */
	return KB_OK;
}
