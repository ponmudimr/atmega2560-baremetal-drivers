/*
 * sw.h - push switch driver, switch to GND, ids and pins in board.h
 * Author: Ponmudi
 */

#ifndef SW_H
#define SW_H

#include "board.h"   /* SW_1, SW_COUNT */

/* set switch pins as input with pull-up, starts timer */
void sw_init(void);

/* 1 while pressed right now (no debounce), 0 if not or wrong id */
unsigned char sw_is_pressed(unsigned char id);

/* 1 once per press (20 ms debounce), call it often */
unsigned char sw_was_pressed(unsigned char id);

#endif
