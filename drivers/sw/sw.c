/*
 * sw.c - push switch driver, switch to GND, ids and pins in board.h
 * Author: Ponmudi
 */

#include "board.h"
#include "gpio.h"
#include "timer.h"
#include "sw.h"

#define SW_DEBOUNCE_MS  20

/* last raw level seen, per switch */
static unsigned char sw_last_raw[SW_COUNT];

/* level that stayed 20 ms, per switch */
static unsigned char sw_stable[SW_COUNT];

/* time of last raw change, per switch */
static unsigned long sw_change_time[SW_COUNT];

/* get port letter for a switch id, 0 if wrong */
static char sw_port(unsigned char id)
{
    switch (id)
    {
        case SW_1: return SW_1_PORT;
        /* new switch: add one more case here */
        default:   return 0;   /* wrong id */
    }
}

/* get pin number for a switch id */
static unsigned char sw_pin(unsigned char id)
{
    switch (id)
    {
        case SW_1: return SW_1_PIN;
        default:   return 0;
    }
}

/* set switch pins as input with pull-up, starts timer */
void sw_init(void)
{
    unsigned char id;

    timer_init();   /* debounce needs timer_millis */

    for (id = 0; id < SW_COUNT; id++)
    {
        gpio_dir(sw_port(id), sw_pin(id), GPIO_IN_PULLUP);
        sw_last_raw[id] = 0;
        sw_stable[id] = 0;
        sw_change_time[id] = 0;
    }
}

/* 1 while pressed right now (no debounce), 0 if not or wrong id */
unsigned char sw_is_pressed(unsigned char id)
{
    if (sw_port(id) == 0)
    {
        return 0;
    }

    /* pressed pulls pin to 0 */
    if (gpio_get(sw_port(id), sw_pin(id)) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* 1 once per press (20 ms debounce), call it often */
unsigned char sw_was_pressed(unsigned char id)
{
    unsigned char raw;

    if (sw_port(id) == 0)
    {
        return 0;
    }

    raw = sw_is_pressed(id);

    if (raw != sw_last_raw[id])
    {
        /* level changed, start the 20 ms wait again */
        sw_last_raw[id] = raw;
        sw_change_time[id] = timer_millis();
        return 0;
    }

    /* same level for 20 ms and it is new */
    if (timer_elapsed(sw_change_time[id], SW_DEBOUNCE_MS) && raw != sw_stable[id])
    {
        sw_stable[id] = raw;

        if (raw == 1)
        {
            return 1;   /* new press */
        }
    }

    return 0;
}
