/*
 * led.c - LED driver, ids and pins are in board.h
 * Author: Ponmudi
 */

#include "board.h"
#include "gpio.h"
#include "led.h"

/* get port letter for an LED id, 0 if wrong */
static char led_port(unsigned char id)
{
    switch (id)
    {
        case LED_SAFE:     return LED_SAFE_PORT;
        case LED_CAUTION:  return LED_CAUTION_PORT;
        case LED_WARNING:  return LED_WARNING_PORT;
        case LED_STOP:     return LED_STOP_PORT;
        case LED_OCCUPIED: return LED_OCCUPIED_PORT;
        default:           return 0;   /* wrong id */
    }
}

/* get pin number for an LED id */
static unsigned char led_pin(unsigned char id)
{
    switch (id)
    {
        case LED_SAFE:     return LED_SAFE_PIN;
        case LED_CAUTION:  return LED_CAUTION_PIN;
        case LED_WARNING:  return LED_WARNING_PIN;
        case LED_STOP:     return LED_STOP_PIN;
        case LED_OCCUPIED: return LED_OCCUPIED_PIN;
        default:           return 0;
    }
}

/* set all LED pins as output, all off */
void led_init(void)
{
    unsigned char id;

    for (id = 0; id < LED_COUNT; id++)
    {
        gpio_dir(led_port(id), led_pin(id), GPIO_OUT);
        gpio_clear(led_port(id), led_pin(id));   /* start off */
    }
}

/* turn one LED on */
void led_on(unsigned char id)
{
    if (led_port(id) == 0)
    {
        return;   /* wrong id */
    }

    gpio_set(led_port(id), led_pin(id));
}

/* turn one LED off */
void led_off(unsigned char id)
{
    if (led_port(id) == 0)
    {
        return;
    }

    gpio_clear(led_port(id), led_pin(id));
}

/* flip one LED */
void led_toggle(unsigned char id)
{
    if (led_port(id) == 0)
    {
        return;
    }

    gpio_invert(led_port(id), led_pin(id));
}

/* turn all LEDs off */
void led_all_off(void)
{
    unsigned char id;

    for (id = 0; id < LED_COUNT; id++)
    {
        led_off(id);
    }
}
