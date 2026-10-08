/*
 * led.h - LED driver, ids and pins are in board.h
 * Author: Ponmudi
 */

#ifndef LED_H
#define LED_H

#include "board.h"   /* LED_SAFE .. LED_OCCUPIED, LED_COUNT */

/* set all LED pins as output, all off */
void led_init(void);

/* turn one LED on */
void led_on(unsigned char id);

/* turn one LED off */
void led_off(unsigned char id);

/* flip one LED */
void led_toggle(unsigned char id);

/* turn all LEDs off */
void led_all_off(void);

#endif
