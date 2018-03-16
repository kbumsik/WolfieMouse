/*
 * kb_hooks.c
 *
 *  Created on: Mar 19, 2017
 *      Author: kbumsik
 */

static void __nothing() {
    // Nothing. Really.
}

void SysTick_hook(void) __attribute__ ((weak, alias("__nothing")));
