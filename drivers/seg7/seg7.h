/*
 * seg7.h - 2-digit 7-segment display, refreshed by Timer0
 * Author: Ponmudi
 */

#ifndef SEG7_H
#define SEG7_H

/* set pins, start 2 ms refresh, interrupts on */
void seg7_init(void);

/* show 0..99, 0..9 uses one digit, above 99 shows dash */
void seg7_show_number(unsigned char num);

/* show "--" */
void seg7_show_dash(void);

#endif
