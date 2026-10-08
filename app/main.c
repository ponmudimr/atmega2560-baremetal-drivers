/*
 * main.c - blink the LED on pin 13 (PB7, on-board "L" LED) every 500 ms
 * Author: Ponmudi
 */

#include "led.h"
#include "timer.h"

#define BLINK_MS  500   /* time between toggles */

int main(void)
{
    unsigned char led13;
    unsigned long last;

    led13 = led_init('B', 7);   /* D13 = PB7 */
    timer_init();               /* 1 ms tick */

    last = timer_millis();

    while (1)
    {
        if (timer_elapsed(last, BLINK_MS))
        {
            last = last + BLINK_MS;   /* keep steady steps */
            led_toggle(led13);
        }
    }

    return 0;
}
