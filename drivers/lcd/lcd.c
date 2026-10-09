/*
 * lcd.c - 16x2 character LCD driver (HD44780 / JHD162A), 4-bit, write only
 * Author: Ponmudi
 */

#include "gpio.h"
#include "lcd.h"

/* HD44780 commands */
#define LCD_CMD_CLEAR      0x01
#define LCD_CMD_ENTRY      0x06   /* cursor moves right after each char */
#define LCD_CMD_DISPLAY    0x0C   /* display on, no cursor, no blink */
#define LCD_CMD_FUNCTION   0x28   /* 4-bit, 2 lines, 5x8 font */
#define LCD_CMD_SET_ADDR   0x80   /* + address */
#define LCD_ROW1_ADDR      0x40   /* row 1 starts here */

/* pins */
static char lcd_rs_port;
static unsigned char lcd_rs_pin;
static char lcd_e_port;
static unsigned char lcd_e_pin;
static char lcd_data_port;
static unsigned char lcd_data_pin;   /* DB4, DB5..DB7 follow */

/* 1 after lcd_init worked */
static unsigned char lcd_ok = 0;

/* wait about n x 1.25 us, busy loop so lcd does not need the timer driver */
static void lcd_wait_us(unsigned int n)
{
    volatile unsigned int count;   /* volatile, so loop is kept */

    /* ~20 cycles per loop = ~1.25 us at 16 MHz */
    for (count = 0; count < n; count++)
    {
    }
}

/* wait about ms milliseconds */
static void lcd_wait_ms(unsigned int ms)
{
    while (ms > 0)
    {
        lcd_wait_us(800);   /* ~1 ms */
        ms--;
    }
}

/* 1 if port letter and pin are valid */
static unsigned char lcd_pin_ok(char port, unsigned char pin)
{
    if (port < 'A' || port > 'L' || port == 'I' || pin > 7)
    {
        return 0;
    }
    return 1;
}

/* put 4 bits on DB4..DB7 and pulse E */
static void lcd_write_nibble(unsigned char nibble)
{
    unsigned char i;

    for (i = 0; i < 4; i++)
    {
        if (nibble & (1 << i))
        {
            gpio_set(lcd_data_port, lcd_data_pin + i);
        }
        else
        {
            gpio_clear(lcd_data_port, lcd_data_pin + i);
        }
    }

    gpio_set(lcd_e_port, lcd_e_pin);     /* LCD reads data on falling E */
    lcd_wait_us(1);
    gpio_clear(lcd_e_port, lcd_e_pin);
    lcd_wait_us(40);                     /* most commands need 37 us */
}

/* send one byte, rs = 0 command, rs = 1 character */
static void lcd_write_byte(unsigned char value, unsigned char rs)
{
    if (rs)
    {
        gpio_set(lcd_rs_port, lcd_rs_pin);
    }
    else
    {
        gpio_clear(lcd_rs_port, lcd_rs_pin);
    }

    lcd_write_nibble(value >> 4);     /* high half first */
    lcd_write_nibble(value & 0x0F);
}

/* set pins, run the start sequence, clear, returns 1 ok or 0 wrong pins */
unsigned char lcd_init(char rs_port, unsigned char rs_pin, char e_port, unsigned char e_pin,
                       char data_port, unsigned char data_first_pin)
{
    unsigned char i;

    /* wrong pins, data needs 4 pins in a row */
    if (!lcd_pin_ok(rs_port, rs_pin) || !lcd_pin_ok(e_port, e_pin) ||
        !lcd_pin_ok(data_port, data_first_pin) || data_first_pin > 4)
    {
        return 0;
    }

    lcd_rs_port = rs_port;
    lcd_rs_pin = rs_pin;
    lcd_e_port = e_port;
    lcd_e_pin = e_pin;
    lcd_data_port = data_port;
    lcd_data_pin = data_first_pin;

    gpio_dir(rs_port, rs_pin, GPIO_OUT);
    gpio_clear(rs_port, rs_pin);
    gpio_dir(e_port, e_pin, GPIO_OUT);
    gpio_clear(e_port, e_pin);
    for (i = 0; i < 4; i++)
    {
        gpio_dir(data_port, data_first_pin + i, GPIO_OUT);
        gpio_clear(data_port, data_first_pin + i);
    }

    /* start sequence from the HD44780 datasheet (4-bit, figure 24) */
    lcd_wait_ms(50);          /* LCD power-up */
    lcd_write_nibble(0x3);
    lcd_wait_ms(5);
    lcd_write_nibble(0x3);
    lcd_wait_us(150);
    lcd_write_nibble(0x3);
    lcd_write_nibble(0x2);    /* now in 4-bit mode */

    lcd_write_byte(LCD_CMD_FUNCTION, 0);
    lcd_write_byte(LCD_CMD_DISPLAY, 0);
    lcd_write_byte(LCD_CMD_ENTRY, 0);

    lcd_ok = 1;
    lcd_clear();

    return 1;
}

/* clear screen, cursor to row 0, col 0 */
void lcd_clear(void)
{
    if (!lcd_ok)
    {
        return;
    }

    lcd_write_byte(LCD_CMD_CLEAR, 0);
    lcd_wait_ms(2);           /* clear needs 1.52 ms */
}

/* move cursor, row 0..1, col 0..15 (wrong values are ignored) */
void lcd_goto(unsigned char row, unsigned char col)
{
    unsigned char addr;

    if (!lcd_ok || row >= LCD_ROWS || col >= LCD_COLS)
    {
        return;
    }

    addr = col;
    if (row == 1)
    {
        addr = LCD_ROW1_ADDR + col;
    }

    lcd_write_byte(LCD_CMD_SET_ADDR | addr, 0);
}

/* write one character at the cursor */
void lcd_putc(char c)
{
    if (!lcd_ok)
    {
        return;
    }

    lcd_write_byte((unsigned char)c, 1);
}

/* write a text at the cursor */
void lcd_print(const char *text)
{
    if (!lcd_ok || text == 0)
    {
        return;
    }

    while (*text != '\0')
    {
        lcd_putc(*text);
        text++;
    }
}

/* write a number 0..65535 at the cursor, no leading zeros */
void lcd_print_number(unsigned int num)
{
    char digits[5];
    unsigned char count = 0;

    /* digits come out backwards, ones first */
    do
    {
        digits[count] = '0' + (num % 10);
        num = num / 10;
        count++;
    } while (num > 0);

    while (count > 0)
    {
        count--;
        lcd_putc(digits[count]);
    }
}
