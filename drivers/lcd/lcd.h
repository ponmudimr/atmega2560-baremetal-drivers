/*
 * lcd.h - 16x2 character LCD driver (HD44780 / JHD162A), 4-bit, write only
 * Author: Ponmudi
 */

#ifndef LCD_H
#define LCD_H

#define LCD_COLS  16
#define LCD_ROWS  2

/* R/W pin goes to GND, only DB4..DB7 are used */
/* data pins: 4 pins in a row on data_port, data_first_pin = DB4 (0..4) */

/* set pins, run the start sequence, clear, returns 1 ok or 0 wrong pins */
unsigned char lcd_init(char rs_port, unsigned char rs_pin, char e_port, unsigned char e_pin,
                       char data_port, unsigned char data_first_pin);

/* clear screen, cursor to row 0, col 0 */
void lcd_clear(void);

/* move cursor, row 0..1, col 0..15 (wrong values are ignored) */
void lcd_goto(unsigned char row, unsigned char col);

/* write one character at the cursor */
void lcd_putc(char c);

/* write a text at the cursor */
void lcd_print(const char *text);

/* write a number 0..65535 at the cursor, no leading zeros */
void lcd_print_number(unsigned int num);

#endif
