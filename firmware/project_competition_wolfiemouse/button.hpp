#ifndef MAIN_BUTTON_HPP_
#define MAIN_BUTTON_HPP_

// Initialize bottun GPIOs and their event handlers
void button_init ();

// Check functions examine if a button pressed.
bool button_b1_check ();
bool button_b2_check ();
bool button_b1_b2_check (); // Check for either buttons

// Wait functions are blocking functions.
// These functions blocks the thread until a button pressed
void button_b1_wait ();
void button_b2_wait ();
void button_b1_b2_wait (); // Wait for either buttons

#endif /* MAIN_BUTTON_HPP_ */
