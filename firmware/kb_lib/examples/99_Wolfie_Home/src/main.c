#include "kb_tick.h"
#include "main.h"
// KB library
#include "system_config.h"
#include "trace.h"
#include "kb_gpio.h"
#include "kb_HCMS-290X_display.h"
#include "kb_terminal.h"
#include "kb_timer.h"

int main(void)
{
	// initialize clock and system configuration
    system_init();

    // Initialize all configured peripherals
    peripheral_init();

    // init peripherals for the LED display
    hcms_290x_init();
    hcms_290x_matrix("STRT");

    // init motion detector
    kb_gpio_init_t motion_cofig = {
    		.Mode = GPIO_MODE_INPUT,
			.Pull = GPIO_PULLDOWN,
			.Speed = GPIO_SPEED_FREQ_LOW
    };
    kb_gpio_init(PORTC, PIN_8, &motion_cofig);

    kb_gpio_toggle(LED1_PORT, LED1_PIN);

    // PWM: PA8
    uint32_t freq;
    kb_pwm_init_t buzzer_config = {
    		.clock_frequency = 10000000, // 1MHz
			.period = 10000				// 2kHz
    };
    freq = 10000000;
    kb_pwm_init(TIMER1, &buzzer_config);
    kb_timer_ch_pin(TIMER1, CH_1, GPIOA, GPIO_PIN_8, NOPULL);
    // start buzzer
    kb_pwm_percent(TIMER1, CH_1, 80);
    kb_pwm_start(TIMER1, CH_1);
    // wait for .5 second
    kb_delay_ms(500);
    // stop buzzer
    kb_pwm_stop(TIMER1, CH_1);

    kb_gpio_toggle(LED1_PORT, LED1_PIN);
    trace_puts("Hello ARM World!");
    kb_terminal_puts("Hello World!\r\n");
    uint32_t seconds = 0;
    while (1)
    {
    	kb_gpio_toggle(LED1_PORT, LED1_PIN);
        kb_terminal_puts("Blink!\r\n");
    	kb_delay_us(1000000);

        ++seconds;
    	if(kb_gpio_read(B1_PORT, B1_PIN) == 0)
    	{
            kb_terminal_puts("Button Pressed!\r\n");
            hcms_290x_matrix("PRSD");
            freq += 1000000;
            if(freq > 20000000)
            {
            	freq = 10000000;
            }
            buzzer_config.clock_frequency = freq;
            kb_pwm_init(TIMER1, &buzzer_config);
    	    kb_pwm_start(TIMER1, CH_1);
    	}
    	else
    	{
            hcms_290x_int(seconds);
    	    kb_pwm_stop(TIMER1, CH_1);
    	}

        // Count seconds on the trace device.
        trace_printf("Second %u\n", seconds);

        // detect motion
        if (kb_gpio_read(PORTC, PIN_8) == 1)
        {
            hcms_290x_matrix("DETC");
        }
    }
}
