/*
 * File    : ex1_gpio_blink.c
 * About   : Example 1 - blink the onboard LED (D13) every 500 ms.
 *           The LED on the Mega board is on PB7 (port 'B', pin 7).
 *           Uses _delay_ms, so the CPU just waits in between.
 * Registers used : DDRB, PORTB (through the GPIO driver)
 * Author  : Ponmudi
 */

#include <avr/io.h>
#include <util/delay.h>
#include "m2560_gpio.h"

#define M2560_LED_PORT  'B'   /* onboard LED port */
#define M2560_LED_PIN   7     /* PB7 = D13 */

int main(void)
{
    /* make the LED pin an output */
    m2560_gpio_dir(M2560_LED_PORT, M2560_LED_PIN, M2560_DIR_OUT);

    while (1)
    {
        m2560_gpio_set(M2560_LED_PORT, M2560_LED_PIN);    /* LED on */
        _delay_ms(500);
        m2560_gpio_clear(M2560_LED_PORT, M2560_LED_PIN);  /* LED off */
        _delay_ms(500);
    }

    return 0;
}
