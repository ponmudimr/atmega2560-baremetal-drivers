/*
 * sw.c - push switch driver, any pin
 * Author: Ponmudi
 */

#include "gpio.h"
#include "timer.h"
#include "sw.h"

#define SW_DEBOUNCE_MS  20

/* pin and type of each added switch */
static char sw_port[SW_MAX];
static unsigned char sw_pin[SW_MAX];
static unsigned char sw_low[SW_MAX];   /* 1 = active low */

/* debounce state, per switch */
static unsigned char sw_last_raw[SW_MAX];
static unsigned char sw_stable[SW_MAX];
static unsigned long sw_change_time[SW_MAX];

/* how many switches are added */
static unsigned char sw_count = 0;

/* add a switch, starts timer, returns id or SW_NONE */
unsigned char sw_init(char port, unsigned char pin, unsigned char active_low)
{
    unsigned char id;

    /* wrong port/pin or table full */
    if (port < 'A' || port > 'L' || port == 'I' || pin > 7 || sw_count >= SW_MAX)
    {
        return SW_NONE;
    }

    id = sw_count;
    sw_port[id] = port;
    sw_pin[id] = pin;
    sw_low[id] = active_low;
    sw_last_raw[id] = 0;
    sw_stable[id] = 0;
    sw_change_time[id] = 0;
    sw_count++;

    if (active_low)
    {
        gpio_dir(port, pin, GPIO_IN_PULLUP);   /* pin high until pressed */
    }
    else
    {
        gpio_dir(port, pin, GPIO_IN);          /* outside pull-down needed */
    }

    timer_init();   /* debounce needs timer_millis */

    return id;
}

/* 1 while pressed right now (no debounce), 0 if not or wrong id */
unsigned char sw_is_pressed(unsigned char id)
{
    unsigned char level;

    if (id >= sw_count)
    {
        return 0;
    }

    level = gpio_get(sw_port[id], sw_pin[id]);

    if (sw_low[id])
    {
        if (level == 0)
        {
            return 1;   /* pressed pulls pin to 0 */
        }
        return 0;
    }
    else
    {
        if (level == 1)
        {
            return 1;   /* pressed pulls pin to 1 */
        }
        return 0;
    }
}

/* 1 once per press (20 ms debounce), call it often */
unsigned char sw_was_pressed(unsigned char id)
{
    unsigned char raw;

    if (id >= sw_count)
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
