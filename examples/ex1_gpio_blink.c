/*
 * ex1_gpio_blink.c - blink onboard LED (D13) every ~500 ms
 * Author: Ponmudi
 */

#include "m2560_gpio.h"

#define M2560_LED_PORT  'B'   /* onboard LED port */
#define M2560_LED_PIN   7     /* PB7 = D13 */

/* rough delay in ms, not exact, no timer needed */
static void m2560_rough_wait(unsigned int ms)
{
    volatile unsigned int count;   /* volatile, so loop is kept */

    while (ms > 0)
    {
        /* ~20 cycles x 800 = ~1 ms */
        for (count = 0; count < 800; count++)
        {
        }
        ms--;
    }
}

int main(void)
{
    /* LED pin as output */
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
