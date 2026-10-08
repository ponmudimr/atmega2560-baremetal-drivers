/*
 * ir.c - IR sensor for parking slot
 * Author: Ponmudi
 */

#include "board.h"
#include "gpio.h"
#include "ir.h"

/* set IR pin as input */
void ir_init(void)
{
    gpio_dir(IR_PORT, IR_PIN, GPIO_IN_PULLUP);   /* pull-up, pin not floating */
}

/* returns 1 if slot is occupied, 0 if free */
unsigned char ir_is_occupied(void)
{
    unsigned char level = gpio_get(IR_PORT, IR_PIN);

    if (IR_ACTIVE_LOW)
    {
        /* sensor gives 0 when car is there */
        if (level == 0)
        {
            return 1;
        }
        return 0;
    }
    else
    {
        if (level == 1)
        {
            return 1;
        }
        return 0;
    }
}
