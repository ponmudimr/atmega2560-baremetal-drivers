/*
 * t_seg7.c - count 00..99 on 7-seg, one step every 300 ms
 * Author: Ponmudi
 */

#include "seg7.h"
#include "timer.h"

int main(void)
{
    unsigned char count = 0;

    seg7_init();
    timer_init();

    while (1)
    {
        seg7_show_number(count);   /* 0..9 one digit, 10..99 two digits */
        timer_delay_ms(300);

        count++;
        if (count > 99)
        {
            count = 0;
        }
    }

    return 0;
}
