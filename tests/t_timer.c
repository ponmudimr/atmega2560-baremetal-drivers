/*
 * t_timer.c - SAFE LED toggles every 1000 ms
 * Author: Ponmudi
 */

#include "led.h"
#include "timer.h"

int main(void)
{
    unsigned long last = 0;

    led_init();
    timer_init();

    while (1)
    {
        if ((timer_millis() - last) >= 1000)
        {
            last = last + 1000;    /* keep steady steps */
            led_toggle(LED_SAFE);
        }
    }

    return 0;
}
