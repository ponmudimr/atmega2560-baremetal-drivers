/*
 * t_ir.c - OCCUPIED LED follows IR #1, 7-seg shows 1 (occupied) or 0 (free)
 * Author: Ponmudi
 */

#include "board.h"
#include "led.h"
#include "ir.h"
#include "seg7.h"

int main(void)
{
    unsigned char occupied_led;
    unsigned char slot_ir;

    occupied_led = led_init(LED_OCCUPIED_PORT, LED_OCCUPIED_PIN);
    slot_ir = ir_init(IR_PORT, IR_PIN, IR_TYPE);
    seg7_init(SEG7_SEG_PORT, SEG7_D1_PORT, SEG7_D1_PIN, SEG7_D2_PORT, SEG7_D2_PIN,
              SEG7_TYPE, SEG7_DIGIT_ON);

    while (1)
    {
        if (ir_is_detected(slot_ir))
        {
            led_on(occupied_led);
            seg7_show_number(1);   /* occupied */
        }
        else
        {
            led_off(occupied_led);
            seg7_show_number(0);   /* free */
        }
    }

    return 0;
}
