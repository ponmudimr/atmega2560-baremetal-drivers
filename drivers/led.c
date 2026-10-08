/*
 * led.c - LED driver for the 5 status LEDs
 * Author: Ponmudi
 */

#include "board.h"
#include "gpio.h"
#include "led.h"

/* get pin number for an LED id, 255 if wrong */
static unsigned char led_pin(unsigned char id)
{
    switch (id)
    {
        case LED_SAFE:     return LED_SAFE_PIN;
        case LED_CAUTION:  return LED_CAUTION_PIN;
        case LED_WARNING:  return LED_WARNING_PIN;
        case LED_STOP:     return LED_STOP_PIN;
        case LED_OCCUPIED: return LED_OCCUPIED_PIN;
        default:           return 255;   /* wrong id */
    }
}

/* set all LED pins as output, all off */
void led_init(void)
{
    unsigned char id;

    for (id = LED_SAFE; id <= LED_OCCUPIED; id++)
    {
        gpio_dir(LED_PORT, led_pin(id), GPIO_OUT);
        gpio_clear(LED_PORT, led_pin(id));   /* start off */
    }
}

/* turn one LED on */
void led_on(unsigned char id)
{
    unsigned char pin = led_pin(id);

    if (pin == 255)
    {
        return;
    }

    gpio_set(LED_PORT, pin);
}

/* turn one LED off */
void led_off(unsigned char id)
{
    unsigned char pin = led_pin(id);

    if (pin == 255)
    {
        return;
    }

    gpio_clear(LED_PORT, pin);
}
