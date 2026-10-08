/*
 * ir.c - IR sensor for parking slot, active level from board.h
 * Author: Ponmudi
 */

#include "board.h"
#include "gpio.h"
#include "ir.h"

/* wait about 1 ms, busy loop so ir does not need the timer driver */
static void ir_wait_1ms(void)
{
    volatile unsigned int count;   /* volatile, so loop is kept */

    /* ~20 cycles x 800 = ~16000 cycles = ~1 ms at 16 MHz */
    for (count = 0; count < 800; count++)
    {
    }
}

/* 1 if this one read says occupied */
static unsigned char ir_read_occupied(void)
{
    unsigned char level = ir_read_raw();

    if (IR_ACTIVE_LOW)
    {
        if (level == 0)
        {
            return 1;   /* low = car there */
        }
        return 0;
    }
    else
    {
        if (level == 1)
        {
            return 1;   /* high = car there */
        }
        return 0;
    }
}

/* set IR pin as input with pull-up */
void ir_init(void)
{
    gpio_dir(IR_PORT, IR_PIN, GPIO_IN_PULLUP);   /* pull-up, pin not floating */
}

/* pin level now, 0 or 1 */
unsigned char ir_read_raw(void)
{
    return gpio_get(IR_PORT, IR_PIN);
}

/* 1 if occupied in 3 reads (~1 ms apart), else 0 */
unsigned char ir_is_occupied(void)
{
    unsigned char i;

    for (i = 0; i < 3; i++)
    {
        if (ir_read_occupied() == 0)
        {
            return 0;   /* one free read is enough to say free */
        }

        if (i < 2)
        {
            ir_wait_1ms();
        }
    }

    return 1;
}
