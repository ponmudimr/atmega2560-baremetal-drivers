/*
 * sw.h - push switch driver (switch to GND)
 * Author: Ponmudi
 */

#ifndef SW_H
#define SW_H

/* set switch pin as input with pull-up */
void sw_init(void);

/* returns 1 when pressed, 0 when not */
unsigned char sw_is_pressed(void);

#endif
