/*
 * seg7.h - 2-digit 7-segment display, refreshed by Timer0
 * Author: Ponmudi
 */

#ifndef SEG7_H
#define SEG7_H

/* pos 0 = left digit (tens), pos 1 = right digit (ones) */

/* set pins, start 2 ms refresh, interrupts on */
void seg7_init(void);

/* show 0..99, 0..9 uses right digit only, above 99 shows dash */
void seg7_show_number(unsigned char num);

/* show 0..15 (0-9, A-F) on one digit */
void seg7_show_digit(unsigned char pos, unsigned char value);

/* show "--" */
void seg7_show_dash(void);

/* turn both digits off (dots too) */
void seg7_blank(void);

/* dot on (1) or off (0) for one digit */
void seg7_set_dp(unsigned char pos, unsigned char on);

/* show own pattern on one digit, bit0 = a ... bit6 = g, bit7 = dp */
void seg7_show_raw(unsigned char pos, unsigned char pattern);

#endif
