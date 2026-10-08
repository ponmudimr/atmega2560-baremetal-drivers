/*
 * t_ir.c - OCCUPIED LED on when slot is occupied
 * Author: Ponmudi
 */

#include "led.h"
#include "ir.h"

int main(void)
{
    led_init();
    ir_init();

    while (1)
    {
        if (ir_is_occupied())
        {
            led_on(LED_OCCUPIED);
        }
        else
        {
            led_off(LED_OCCUPIED);
        }
    }

    return 0;
}
