/*
 * led.h - LED driver, any pin, LED on = pin high
 * Author: Ponmudi
 */

#ifndef LED_H
#define LED_H

#define LED_MAX   8     /* max LEDs */
#define LED_NONE  255   /* led_init failed */

/* add an LED on port/pin, starts off, returns id or LED_NONE */
unsigned char led_init(char port, unsigned char pin);

/* turn LED on */
void led_on(unsigned char id);

/* turn LED off */
void led_off(unsigned char id);

/* flip LED */
void led_toggle(unsigned char id);

/* turn all added LEDs off */
void led_all_off(void);

#endif
