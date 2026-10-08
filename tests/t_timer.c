/*
 * t_timer.c - SAFE LED toggles every 1000 ms
 * Author: Ponmudi
 */

#include "led.h"
#include "timer.h"

int main(void)
{
    unsigned long last = 0;
    unsigned char led_state = 0;   /* 0 = off, 1 = on */

    led_init();
    timer_init();

    while (1)
    {
        if ((timer_millis() - last) >= 1000)
        {
            last = last + 1000;    /* keep steady steps */

            if (led_state == 0)
            {
                led_on(LED_SAFE);
                led_state = 1;
            }
            else
            {
                led_off(LED_SAFE);
                led_state = 0;
            }
        }
    }

    return 0;
}
