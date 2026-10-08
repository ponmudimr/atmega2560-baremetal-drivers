/*
 * File    : ex4_adc_pot_dimmer.c
 * About   : Example 4 - a potentiometer on A0 sets the LED brightness on D6.
 *           Every 20 ms: read ADC channel 0 (0..1023), shift right by 2
 *           to get 0..255, and use that as the PWM duty.
 *           Uses all four drivers together.
 *           Wiring: pot outer pins to 5V and GND, middle pin to A0.
 *                   D6 -> 220 ohm resistor -> LED (+), LED (-) -> GND.
 * Registers used : ADC (ADC driver), Timer4 (PWM driver),
 *                  Timer1 + SREG (timer driver), DDRH (GPIO driver)
 * Author  : Ponmudi
 */

#include "m2560_timer.h"
#include "m2560_pwm.h"
#include "m2560_adc.h"

#define M2560_POT_CHANNEL   0    /* A0 */
#define M2560_READ_EVERY_MS 20

int main(void)
{
    unsigned long last_time = 0;
    unsigned int raw;

    m2560_timer_init();
    m2560_adc_init();
    m2560_pwm_init('A');    /* D6, uses the GPIO driver inside */

    while (1)
    {
        if ((m2560_timer_millis() - last_time) >= M2560_READ_EVERY_MS)
        {
            last_time = last_time + M2560_READ_EVERY_MS;

            raw = m2560_adc_sample(M2560_POT_CHANNEL);   /* 0..1023 */

            /* 10-bit to 8-bit: drop the 2 lowest bits (divide by 4) */
            m2560_pwm_duty('A', (unsigned char)(raw >> 2));
        }
    }

    return 0;
}
