/*
 * t_gate.c - gate demo: occupied -> dash + LED, else distance on 7-seg
 * Author: Ponmudi
 */

#include "led.h"
#include "ir.h"
#include "ultra.h"
#include "seg7.h"
#include "timer.h"

int main(void)
{
    unsigned int cm;

    led_init();
    ir_init();
    ultra_init();
    seg7_init();
    timer_init();

    while (1)
    {
        if (ir_is_occupied())
        {
            led_on(LED_OCCUPIED);
            seg7_show_dash();
        }
        else
        {
            led_off(LED_OCCUPIED);

            cm = ultra_get_cm();
            if (cm > 99)
            {
                seg7_show_dash();   /* too far or no echo */
            }
            else
            {
                seg7_show_number((unsigned char)cm);
            }
        }

        timer_delay_ms(100);
    }

    return 0;
}
