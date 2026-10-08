/*
 * sw.h - push switch driver, any pin
 * Author: Ponmudi
 */

#ifndef SW_H
#define SW_H

#define SW_MAX          4     /* max switches */
#define SW_NONE         255   /* sw_init failed */
#define SW_ACTIVE_LOW   1     /* switch to GND, pull-up turned on */
#define SW_ACTIVE_HIGH  0     /* switch to 5V, needs pull-down resistor */

/* add a switch, starts timer, returns id or SW_NONE */
unsigned char sw_init(char port, unsigned char pin, unsigned char active_low);

/* 1 while pressed right now (no debounce), 0 if not or wrong id */
unsigned char sw_is_pressed(unsigned char id);

/* 1 once per press (20 ms debounce), call it often */
unsigned char sw_was_pressed(unsigned char id);

#endif
