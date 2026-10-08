/*
 * t_led_sw.c - LEDs on one by one, STOP LED follows switch
 * Author: Ponmudi
 */

#include "led.h"
#include "sw.h"
#include "timer.h"

int main(void)
{
    unsigned char id;

    led_init();
    sw_init();
    timer_init();

    /* LEDs on one by one */
    for (id = 0; id < LED_COUNT; id++)
    {
        led_on(id);
        timer_delay_ms(300);
    }

    led_all_off();

    while (1)
    {
        if (sw_is_pressed())
        {
            led_on(LED_STOP);
        }
        else
        {
            led_off(LED_STOP);
        }
    }

    return 0;
}
