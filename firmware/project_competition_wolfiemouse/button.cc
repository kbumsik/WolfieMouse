#include "button.hpp"
#include "module_config.h"
#include "gpio.h"
#include "trace.h"

// Event declarations
static void _on_b1_pressed(void);
static void _on_b2_pressed(void);

// Local variables
static volatile int _is_b1_pressed = 0;
static volatile int _is_b2_pressed = 0;

/*******************************************************************************
 * Function definitions
 ******************************************************************************/
// Initialize bottun GPIOs and their event handlers
void button_init ()
{
    gpio_init_t GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = PULLUP;

    gpio_isr_enable(B1_PORT, B1_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B1_PORT, B1_PIN, _on_b1_pressed);

    gpio_isr_enable(B2_PORT, B2_PIN, &GPIO_InitStruct, FALLING_EDGE);
    gpio_isr_register(B2_PORT, B2_PIN, _on_b2_pressed);
}

// Check functions examine if a button pressed.
bool button_b1_check ()
{
    if (_is_b1_pressed) {
        _is_b1_pressed = 0;
        return true;
    }
    return false;
}
bool button_b2_check ()
{
    if (_is_b2_pressed) {
        _is_b2_pressed = 0;
        return true;
    }
    return false;
}

bool button_b1_b2_check ()
{
    return button_b1_check() || button_b2_check();
}

// Wait functions are blocking functions.
// These functions blocks the thread until a button pressed
void button_b1_wait ()
{
    while (!_is_b1_pressed) {
        // Do nothing, just block
    }
    _is_b1_pressed = 0;
}

void button_b2_wait ()
{
    while (!_is_b2_pressed) {
        // Do nothing, just block
    }
    _is_b2_pressed = 0;
}

void button_b1_b2_wait ()
{
    while (!_is_b1_pressed && !_is_b2_pressed) {
        // Do nothing, just block
    }
    _is_b1_pressed = 0;
    _is_b2_pressed = 0;
}

/*******************************************************************************
 * Event handlers
 ******************************************************************************/
static void _on_b1_pressed(void)
{
    _is_b1_pressed = 1;
    trace_puts("B1 Pressed\n");
    return;
}

static void _on_b2_pressed(void)
{
    _is_b2_pressed = 1;
    trace_puts("B2 Pressed\n");
    return;
}
