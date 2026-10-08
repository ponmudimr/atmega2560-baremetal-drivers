/*
 * t_timer.c - SAFE LED toggles every 1000 ms
 * Author: Ponmudi
 */

#include "board.h"
#include "led.h"
#include "timer.h"

int main(void)
{
    unsigned long last = 0;
    unsigned char safe;

    safe = led_init(LED_SAFE_PORT, LED_SAFE_PIN);
    timer_init();

    while (1)
    {
        if ((timer_millis() - last) >= 1000)
        {
            last = last + 1000;    /* keep steady steps */
            led_toggle(safe);
        }
    }

    return 0;
}
