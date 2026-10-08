/*
 * t_pwm.c - buzzer at 25%, 50%, 75% duty, 1 s each
 * Author: Ponmudi
 */

#include "pwm.h"
#include "timer.h"

int main(void)
{
    pwm_init();
    timer_init();
    pwm_on();

    while (1)
    {
        pwm_set_duty(25);
        timer_delay_ms(1000);

        pwm_set_duty(50);
        timer_delay_ms(1000);

        pwm_set_duty(75);
        timer_delay_ms(1000);
    }

    return 0;
}
