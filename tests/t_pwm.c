/*
 * t_pwm.c - buzzer on channel A (D6) at 25%, 50%, 75% duty, 1 s each
 * Author: Ponmudi
 */

#include "pwm.h"
#include "timer.h"

int main(void)
{
    pwm_init('A');
    timer_init();
    pwm_on('A');

    while (1)
    {
        pwm_set_duty('A', 25);
        timer_delay_ms(1000);

        pwm_set_duty('A', 50);
        timer_delay_ms(1000);

        pwm_set_duty('A', 75);
        timer_delay_ms(1000);
    }

    return 0;
}
