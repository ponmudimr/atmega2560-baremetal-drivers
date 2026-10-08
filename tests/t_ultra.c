/*
 * t_ultra.c - distance on 7-seg every 100 ms, dash if > 99 or no echo
 * Author: Ponmudi
 */

#include "board.h"
#include "ultra.h"
#include "seg7.h"
#include "timer.h"

int main(void)
{
    unsigned int cm;
    unsigned char rear;

    rear = ultra_init(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, ULTRA_ECHO_PORT, ULTRA_ECHO_PIN);
    seg7_init();
    timer_init();

    while (1)
    {
        cm = ultra_get_cm(rear);

        if (cm > 99)
        {
            seg7_show_dash();   /* too far or no echo (999) */
        }
        else
        {
            seg7_show_number((unsigned char)cm);
        }

        timer_delay_ms(100);
    }

    return 0;
}
