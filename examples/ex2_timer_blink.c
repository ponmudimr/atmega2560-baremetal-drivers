/*
 * File    : ex2_timer_blink.c
 * About   : Example 2 - toggle the onboard LED (D13 = PB7) every 1000 ms
 *           using m2560_timer_millis(). The loop never waits, it only
 *           checks the time, so other code could run in the loop too.
 * Registers used : Timer1 + SREG (timer driver), DDRB/PORTB (GPIO driver)
 * Author  : Ponmudi
 */

#include "m2560_gpio.h"
#include "m2560_timer.h"

#define M2560_LED_PORT   'B'
#define M2560_LED_PIN    7
#define M2560_BLINK_MS   1000

int main(void)
{
    unsigned long last_time = 0;

    m2560_gpio_dir(M2560_LED_PORT, M2560_LED_PIN, M2560_DIR_OUT);
    m2560_timer_init();    /* 1 ms tick + interrupts on */

    while (1)
    {
        /* has 1000 ms passed since the last toggle? */
        if ((m2560_timer_millis() - last_time) >= M2560_BLINK_MS)
        {
            last_time = last_time + M2560_BLINK_MS;   /* keeps steady timing */
            m2560_gpio_invert(M2560_LED_PORT, M2560_LED_PIN);
        }

        /* other work could go here, nothing is blocked */
    }

    return 0;
}
