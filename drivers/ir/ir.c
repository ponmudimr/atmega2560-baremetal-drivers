/*
 * ir.c - IR obstacle sensor driver, any pin
 * Author: Ponmudi
 */

#include "gpio.h"
#include "ir.h"

/* pin and type of each added sensor */
static char ir_port[IR_MAX];
static unsigned char ir_pin[IR_MAX];
static unsigned char ir_low[IR_MAX];   /* 1 = active low */

/* how many sensors are added */
static unsigned char ir_count = 0;

/* wait about 1 ms, busy loop so ir does not need the timer driver */
static void ir_wait_1ms(void)
{
    volatile unsigned int count;   /* volatile, so loop is kept */

    /* ~20 cycles x 800 = ~16000 cycles = ~1 ms at 16 MHz */
    for (count = 0; count < 800; count++)
    {
    }
}

/* 1 if this one read sees an object */
static unsigned char ir_read_once(unsigned char id)
{
    unsigned char level = ir_read_raw(id);

    if (ir_low[id])
    {
        if (level == 0)
        {
            return 1;   /* low = object */
        }
        return 0;
    }
    else
    {
        if (level == 1)
        {
            return 1;   /* high = object */
        }
        return 0;
    }
}

/* add a sensor, returns id or IR_NONE */
unsigned char ir_init(char port, unsigned char pin, unsigned char active_low)
{
    unsigned char id;

    /* wrong port/pin or table full */
    if (port < 'A' || port > 'L' || port == 'I' || pin > 7 || ir_count >= IR_MAX)
    {
        return IR_NONE;
    }

    id = ir_count;
    ir_port[id] = port;
    ir_pin[id] = pin;
    ir_low[id] = active_low;
    ir_count++;

    gpio_dir(port, pin, GPIO_IN_PULLUP);   /* pull-up, pin not floating */

    return id;
}

/* pin level now, 0 or 1 (0 if wrong id) */
unsigned char ir_read_raw(unsigned char id)
{
    if (id >= ir_count)
    {
        return 0;
    }

    return gpio_get(ir_port[id], ir_pin[id]);
}

/* 1 if object seen in 3 reads (~1 ms apart), else 0 */
unsigned char ir_is_detected(unsigned char id)
{
    unsigned char i;

    if (id >= ir_count)
    {
        return 0;
    }

    for (i = 0; i < 3; i++)
    {
        if (ir_read_once(id) == 0)
        {
            return 0;   /* one clear read is enough to say no */
        }

        if (i < 2)
        {
            ir_wait_1ms();
        }
    }

    return 1;
}
