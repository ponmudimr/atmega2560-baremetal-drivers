/*
 * t_adc.c - pot value / 11 on 7-seg (0..93)
 * Author: Ponmudi
 */

#include "board.h"
#include "adc.h"
#include "seg7.h"
#include "timer.h"

int main(void)
{
    unsigned int raw;

    adc_init();
    seg7_init(SEG7_SEG_PORT, SEG7_D1_PORT, SEG7_D1_PIN, SEG7_D2_PORT, SEG7_D2_PIN,
              SEG7_TYPE, SEG7_DIGIT_ON);
    timer_init();

    while (1)
    {
        raw = adc_read(POT_CHANNEL);              /* 0..1023 */
        seg7_show_number((unsigned char)(raw / 11));
        timer_delay_ms(100);
    }

    return 0;
}
