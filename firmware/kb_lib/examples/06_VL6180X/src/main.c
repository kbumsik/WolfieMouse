#include "kb_tick.h"
#include "main.h"
// KB library
#include "system_config.h"
#include "trace.h"
#include "kb_gpio.h"
#include "kb_terminal.h"
#include "kb_timer.h"
#include "kb_TCA9545A_i2c_mux.h"

int main(void)
{
	// initialize clock and system configuration
    system_init();

    // Initialize all configured peripherals
    peripheral_init();

    // init i2c
    tca9545a_init();
    tca9545a_select_ch(1);

    kb_gpio_toggle(LED1_PORT, LED1_PIN);
    trace_puts("Hello ARM World!");
    kb_terminal_puts("Hello World!\r\n");
    uint8_t seconds = 0;
    while (1)
    {
    	tca9545a_current_ch();
    	kb_gpio_toggle(LED1_PORT, LED1_PIN);
        kb_terminal_puts("Blink!\r\n");
    	kb_delay_us(1000000);

        ++seconds;

        // Count seconds on the trace device.
        trace_printf("Second %u\n", seconds);
    }
}
