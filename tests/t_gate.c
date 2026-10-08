/*
 * t_gate.c - gate demo: occupied -> dash + LED, else distance on 7-seg
 * Author: Ponmudi
 */

#include "board.h"
#include "led.h"
#include "ir.h"
#include "ultra.h"
#include "seg7.h"
#include "timer.h"

int main(void)
{
    unsigned int cm;
    unsigned char rear;
    unsigned char occupied_led;
    unsigned char slot_ir;

    occupied_led = led_init(LED_OCCUPIED_PORT, LED_OCCUPIED_PIN);
    slot_ir = ir_init(IR_PORT, IR_PIN, IR_TYPE);
    rear = ultra_init(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, ULTRA_ECHO_PORT, ULTRA_ECHO_PIN);
    seg7_init();
    timer_init();

    while (1)
    {
        if (ir_is_detected(slot_ir))
        {
            led_on(occupied_led);
            seg7_show_dash();
        }
        else
        {
            led_off(occupied_led);

            cm = ultra_get_cm(rear);
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
