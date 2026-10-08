/*
 * main.c - blink the LED on pin 13 (PB7, on-board "L" LED), for-loop delay
 * Author: Ponmudi
 */

#include "gpio.h"

#define LED_PORT  'B'
#define LED_PIN   7     /* D13 = PB7 */

/* wait about ms milliseconds, not exact */
static void blink_wait_ms(unsigned int ms)
{
    unsigned int i;
    volatile unsigned int count;   /* volatile, so loop is kept */

    for (i = 0; i < ms; i++)
    {
        /* ~20 cycles x 800 = ~16000 cycles = ~1 ms at 16 MHz */
        for (count = 0; count < 800; count++)
        {
        }
    }
}

int main(void)
{
    gpio_dir(LED_PORT, LED_PIN, GPIO_OUT);

    for (;;)
    {
        gpio_set(LED_PORT, LED_PIN);     /* LED on */
        blink_wait_ms(500);
        gpio_clear(LED_PORT, LED_PIN);   /* LED off */
        blink_wait_ms(500);
    }

    return 0;
}
