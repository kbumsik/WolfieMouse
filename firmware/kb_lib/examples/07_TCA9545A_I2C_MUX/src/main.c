#include "kb_tick.h"
#include "main.h"
// KB library
#include "system_config.h"
#include "trace.h"
#include "kb_gpio.h"
#include "kb_terminal.h"
#include "kb_timer.h"
#include "kb_TCA9545A_i2c_mux.h"

void print_channel(uint8_t ch);

int main(void)
{
	uint8_t channel;
	// initialize clock and system configuration
    system_init();

    // Initialize all configured peripherals
    peripheral_init();

    // init i2c
    tca9545a_init();
    tca9545a_select_ch(1);

    /* welcome message */
    kb_gpio_toggle(LED1_PORT, LED1_PIN);
    trace_puts("Hello ARM World!");
    //kb_terminal_puts("Hello World!\r\n");

    /* select CH0 */
	kb_delay_us(1000000);
	tca9545a_select_ch(TCA9545A_CH_0);
	channel = tca9545a_current_ch();
	print_channel(channel); // double check

    /* select CH1 */
	kb_delay_us(1000000);
	tca9545a_select_ch(TCA9545A_CH_1);
	channel = tca9545a_current_ch();
	print_channel(channel); // double check

    /* select CH2 */
	kb_delay_us(1000000);
	tca9545a_select_ch(TCA9545A_CH_2);
	channel = tca9545a_current_ch();
	print_channel(channel); // double check

    /* select CH3 */
	kb_delay_us(1000000);
	tca9545a_select_ch(TCA9545A_CH_3);
	channel = tca9545a_current_ch();
	print_channel(channel); // double check

    /* select multiple channel */
	kb_delay_us(1000000);
	tca9545a_select_ch(TCA9545A_CH_0|TCA9545A_CH_3);
	channel = tca9545a_current_ch();
	print_channel(channel); // double check

    /* get interrupted channel */
	kb_delay_us(1000000);
	channel = tca9545a_current_it();
	//kb_terminal_puts("Interrupted channels : \r\n");
	print_channel(channel); // double check
	tca9545a_clear_it(channel);	// clear channel
	// ..or
	tca9545a_clear_it(TCA9545A_CH_0|TCA9545A_CH_1|TCA9545A_CH_2|TCA9545A_CH_3);


    //kb_terminal_puts("This completes the example\r\n");
}

void print_channel(uint8_t ch)
{
	if (ch & TCA9545A_CH_0)
	{
		//kb_terminal_puts("CH_0\r\n");
	}
	if (ch & TCA9545A_CH_1)
	{
		//kb_terminal_puts("CH_1\r\n");
	}
	if (ch & TCA9545A_CH_2)
	{
		//kb_terminal_puts("CH_2\r\n");
	}
	if (ch & TCA9545A_CH_3)
	{
		//kb_terminal_puts("CH_3\r\n");
	}
}
