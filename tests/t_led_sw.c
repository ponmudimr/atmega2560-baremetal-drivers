/*
 * t_led_sw.c - LEDs on one by one, STOP LED follows switch
 * Author: Ponmudi
 */

#include "board.h"
#include "led.h"
#include "sw.h"
#include "timer.h"

int main(void)
{
    unsigned char leds[5];
    unsigned char key;
    unsigned char i;

    /* order: safe, caution, warning, stop, occupied */
    leds[0] = led_init(LED_SAFE_PORT, LED_SAFE_PIN);
    leds[1] = led_init(LED_CAUTION_PORT, LED_CAUTION_PIN);
    leds[2] = led_init(LED_WARNING_PORT, LED_WARNING_PIN);
    leds[3] = led_init(LED_STOP_PORT, LED_STOP_PIN);
    leds[4] = led_init(LED_OCCUPIED_PORT, LED_OCCUPIED_PIN);
    key = sw_init(SW_PORT, SW_PIN, SW_ACTIVE_LOW);
    timer_init();

    /* LEDs on one by one */
    for (i = 0; i < 5; i++)
    {
        led_on(leds[i]);
        timer_delay_ms(300);
    }

    led_all_off();

    while (1)
    {
        if (sw_is_pressed(key))
        {
            led_on(leds[3]);   /* STOP */
        }
        else
        {
            led_off(leds[3]);
        }
    }

    return 0;
}
