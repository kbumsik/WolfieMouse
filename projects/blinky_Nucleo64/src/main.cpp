#include "kb_trace.h"
#include "kb_tick.h"
// KB library
#include "system_config.h"
#include "kb_gpio.h"
#include "kb_terminal.h"
#include "kb_adc.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "portable.h" // TODO: Delete
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

void on_pressed(void);

static void task_blinky(void *pvParameters);

/* Task Handlers */
TaskHandle_t task_blinky_handler;

/* peripheral objects */
kb_adc_t range_front_right;

int main(void)
{
    // initialize clock and system configuration
    system_init();

    // Initialize all configured peripherals
    peripheral_init();

    // ADC
    kb_adc_init_t adc_init = {
            .device = RECV_ADC,
            .channel = KB_ADC_CH10
    };
    kb_adc_init(&range_front_right, &adc_init);
    kb_adc_pin(RECV_FR_PORT, RECV_FR_PIN);

    // Set interrupt button
    kb_gpio_init_t GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = NOPULL;
    kb_gpio_isr_enable(B1_PORT, B1_PIN, &GPIO_InitStruct, RISING_EDGE);
    kb_gpio_isr_register(B1_PORT, B1_PIN, on_pressed);

    // Set toggling pin controlled by the button
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    kb_gpio_init(PORTA, PIN_0, &GPIO_InitStruct);
    kb_gpio_set(PORTA, PIN_0, GPIO_PIN_RESET);

    // Toggle LED once
    kb_gpio_toggle(LED1_PORT, LED1_PIN);

    // wait for .5 second
    kb_delay_ms(500);
    kb_gpio_toggle(LED1_PORT, LED1_PIN);

    trace_puts("Hello ARM World!");
    kb_terminal_puts("Hello World!\n");

    BaseType_t result;
    /* definition and creation of defaultTask */
    result = xTaskCreate(
            task_blinky,            /* Pointer to the function that implements the task */
            "Blinky",               /* Text name for the task. This is to facilitate debugging only. It is not used in the scheduler */
            configMINIMAL_STACK_SIZE, /* Stack depth in words */
            NULL,                   /* Pointer to a task parameters */
            1,                      /* The task priority */
            &task_blinky_handler);  /* Pointer of its task handler, if you don't want to use, you can leave it NULL */

    if (result != pdPASS) {
        KB_DEBUG_ERROR("Creating task failed!!");
    }
    /* Do not put delay function in this section!
     * Because xTaskCreate will stop systick until the scheduler called */
    // TODO: check if it is true
    /* Start scheduler */
    vTaskStartScheduler();
    while (1) {
    }
}

void on_pressed(void)
{
    trace_puts("Button Pressed\n");
    kb_gpio_toggle(PORTA, PIN_0);
    uint32_t result = kb_adc_measure(&range_front_right);
    trace_printf("result: %d\n", result);
    return;
}

/* vBlinkyTask function */
void task_blinky(void *pvParameters)
{
    portTickType xLastWakeTime;
    /* Initialize xLastWakeTime for vTaskDelayUntil */
    /* This variable is updated every vTaskDelayUntil is called */
    xLastWakeTime = xTaskGetTickCount();

    uint32_t seconds = 0;

    while (1) {
        kb_gpio_toggle(LED1_PORT, LED1_PIN);
        ++seconds;
        // Count seconds on the trace device.
        trace_printf("Second %u\n", seconds);
        /* Call this Task explicitly every 50ms ,NOT Delay for 50ms */
        vTaskDelayUntil(&xLastWakeTime, (500 / portTICK_RATE_MS));
    }

    /* It never goes here, but the task should be deleted when it reached here */
    vTaskDelete(NULL);
}
