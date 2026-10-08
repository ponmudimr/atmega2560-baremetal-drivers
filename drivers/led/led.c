/*
 * led.c - LED driver, any pin, LED on = pin high
 * Author: Ponmudi
 */

#include "gpio.h"
#include "led.h"

/* port and pin of each added LED */
static char led_port[LED_MAX];
static unsigned char led_pin[LED_MAX];

/* how many LEDs are added */
static unsigned char led_count = 0;

/* add an LED on port/pin, starts off, returns id or LED_NONE */
unsigned char led_init(char port, unsigned char pin)
{
    unsigned char id;

    /* wrong port/pin or table full */
    if (port < 'A' || port > 'L' || port == 'I' || pin > 7 || led_count >= LED_MAX)
    {
        return LED_NONE;
    }

    id = led_count;
    led_port[id] = port;
    led_pin[id] = pin;
    led_count++;

    gpio_dir(port, pin, GPIO_OUT);
    gpio_clear(port, pin);   /* start off */

    return id;
}

/* turn LED on */
void led_on(unsigned char id)
{
    if (id >= led_count)
    {
        return;   /* wrong id */
    }

    gpio_set(led_port[id], led_pin[id]);
}

/* turn LED off */
void led_off(unsigned char id)
{
    if (id >= led_count)
    {
        return;
    }

    gpio_clear(led_port[id], led_pin[id]);
}

/* flip LED */
void led_toggle(unsigned char id)
{
    if (id >= led_count)
    {
        return;
    }

    gpio_invert(led_port[id], led_pin[id]);
}

/* turn all added LEDs off */
void led_all_off(void)
{
    unsigned char id;

    for (id = 0; id < led_count; id++)
    {
        led_off(id);
    }
}
