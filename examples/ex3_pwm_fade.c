/*
 * File    : ex3_pwm_fade.c
 * About   : Example 3 - fade an LED on D6 up and down smoothly.
 *           Wiring: D6 -> 220 ohm resistor -> LED (+), LED (-) -> GND.
 *           One full fade up takes 256 steps * 4 ms = about 1 second.
 * Registers used : Timer4 (PWM driver), Timer1 + SREG (timer driver)
 * Author  : Ponmudi
 */

#include "m2560_timer.h"
#include "m2560_pwm.h"

#define M2560_FADE_STEP_MS  4    /* time between brightness steps */

int main(void)
{
    unsigned int level;

    m2560_timer_init();      /* for m2560_timer_wait_ms() */
    m2560_pwm_init('A');     /* channel A = OC4A = D6 */

    while (1)
    {
        /* brighter: 0 up to 255 */
        for (level = 0; level <= 255; level++)
        {
            m2560_pwm_duty('A', (unsigned char)level);
            m2560_timer_wait_ms(M2560_FADE_STEP_MS);
        }

        /* dimmer: 255 down to 0 */
        for (level = 255; level > 0; level--)
        {
            m2560_pwm_duty('A', (unsigned char)level);
            m2560_timer_wait_ms(M2560_FADE_STEP_MS);
        }
    }

    return 0;
}
