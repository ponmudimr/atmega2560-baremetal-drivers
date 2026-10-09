/*
 * keypad.h - 4x4 matrix keypad driver, any pins
 * Author: Ponmudi
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#define KEYPAD_NO_KEY  0   /* no key pressed */

/* keys:  1 2 3 A  (row 1)
          4 5 6 B
          7 8 9 C
          * 0 # D  (row 4) */

/* rows R1..R4: 4 pins in a row on row_port from row_first_pin (0..4) */
/* cols L1..L4: 4 pins in a row on col_port from col_first_pin (0..4) */

/* set pins, starts timer, returns 1 ok or 0 wrong pins */
unsigned char keypad_init(char row_port, unsigned char row_first_pin,
                          char col_port, unsigned char col_first_pin);

/* key held now (no debounce), or KEYPAD_NO_KEY */
char keypad_get_key(void);

/* key once per press (20 ms debounce), or KEYPAD_NO_KEY, call it often */
char keypad_was_pressed(void);

#endif
