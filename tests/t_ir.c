/*
 * t_ir.c - OCCUPIED LED on when slot is occupied
 * Author: Ponmudi
 */

#include "board.h"
#include "led.h"
#include "ir.h"

int main(void)
{
    unsigned char occupied_led;

    occupied_led = led_init(LED_OCCUPIED_PORT, LED_OCCUPIED_PIN);
    ir_init();

    while (1)
    {
        if (ir_is_occupied())
        {
            led_on(occupied_led);
        }
        else
        {
            led_off(occupied_led);
        }
    }

    return 0;
}
