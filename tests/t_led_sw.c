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
    for (id = LED_SAFE; id <= LED_OCCUPIED; id++)
    {
        led_on(id);
        timer_delay_ms(300);
    }

    /* all off */
    for (id = LED_SAFE; id <= LED_OCCUPIED; id++)
    {
        led_off(id);
    }

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
