#include "kb_tick.h"
// KB library
#include "system_config.h"
#include "kb_trace.h"
#include "kb_gpio.h"
#include "kb_HCMS-290X_display.h"
#include "kb_terminal.h"
#include "kb_timer.h"
#include "kb_TCA9545A_i2c_mux.h"
#include "kb_VL6180X_range_finder.h"
#include "encoder.h"
#include "motor.h"

#ifdef KB_MSG_BASE
	#undef KB_MSG_BASE
	#define KB_MSG_BASE "Main"
#endif

int main(void)
{
	// initialize clock and system configuration
    system_init();

    // Initialize all configured peripherals
    peripheral_init();

    // init terminal
    kb_terminal_init();

    // init peripherals for the LED display
    hcms_290x_init();
    hcms_290x_matrix("STRT");

    // init i2c mux
    tca9545a_init();
    tca9545a_select_ch(TCA9545A_CH_0);

    // init vl6180x
    vl6180x_init();
    vl6180x_range_mm();

    // init encoder
    encoder_init();
    int left = encoder_left_count();
    int right = encoder_right_count();
    KB_DEBUG_MSG("left encoder: %d\r\n", left);
    KB_DEBUG_MSG("right encoder: %d\r\n", right);

    // init motor
    motor_init();
    motor_speed_percent(CH_BOTH, 10);
    // motor_start(CH_BOTH);

    kb_gpio_toggle(LED4_PORT, LED4_PIN);
    // wait for .5 second
    kb_delay_ms(500);

    kb_gpio_toggle(LED4_PORT, LED4_PIN);
    trace_puts("Hello ARM World!");
    kb_terminal_puts("Hello World!\r\n");


    // motor_stop(CH_BOTH);
    uint32_t seconds = 0;
    while (1)
    {
    	kb_gpio_toggle(LED4_PORT, LED4_PIN);
        kb_terminal_puts("Blink!\r\n");
    	kb_delay_us(1000000);

        hcms_290x_int(seconds);
        ++seconds;        // Count seconds on the trace device.
        trace_printf("Second %u\n", seconds);
    }
}
