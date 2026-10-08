/*
 * File    : ex1_gpio_blink.c
 * About   : Example 1 - blink the onboard LED (D13) about every 500 ms.
 *           The LED on the Mega board is on PB7 (port 'B', pin 7).
 *           Uses a simple busy loop for the wait, so the CPU just
 *           waits in between.
 * Registers used : DDRB, PORTB (through the GPIO driver)
 * Author  : Ponmudi
 */

#include "m2560_gpio.h"

#define M2560_LED_PORT  'B'   /* onboard LED port */
#define M2560_LED_PIN   7     /* PB7 = D13 */

/*
 * m2560_rough_wait - waste time in a loop, roughly ms milliseconds
 * ms : about how many milliseconds to wait
 * returns nothing
 * This is only APPROXIMATE (depends on the compiler and -Os).
 * It is here only so this GPIO example does not need the timer
 * driver. Other examples use m2560_timer_wait_ms() instead.
 */
static void m2560_rough_wait(unsigned int ms)
{
    volatile unsigned int count;   /* volatile so the compiler keeps the loop */

    while (ms > 0)
    {
        /* one round is about 20 CPU cycles with -Os (checked with
           avr-objdump), 800 * 20 = 16000 cycles = about 1 ms at 16 MHz */
        for (count = 0; count < 800; count++)
        {
        }
        ms--;
    }
}

int main(void)
{
    /* make the LED pin an output */
    m2560_gpio_dir(M2560_LED_PORT, M2560_LED_PIN, M2560_DIR_OUT);

    while (1)
    {
        m2560_gpio_set(M2560_LED_PORT, M2560_LED_PIN);    /* LED on */
        m2560_rough_wait(500);
        m2560_gpio_clear(M2560_LED_PORT, M2560_LED_PIN);  /* LED off */
        m2560_rough_wait(500);
    }

    return 0;
}
