#include "kb_module_config.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "system_config.h"
#include "kb_trace.h"
#include "kb_tick.h"

int main(void)
{
	// initialize clock and system configuration
    system_init();

    // Initialize all configured peripherals
    peripheral_init();

    HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN);
    kb_delay_ms(500);
    HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN);
    trace_puts("Hello ARM World!");
    uint32_t seconds = 0;
    while (1)
    {
    	HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN);
    	kb_delay_us(1000000);

        ++seconds;
        // Count seconds on the trace device.
        trace_printf("Second %u\n", seconds);
    }
}
