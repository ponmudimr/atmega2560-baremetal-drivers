/*
 * keypad.c - 4x4 matrix keypad driver, any pins
 * Author: Ponmudi
 */

#include "gpio.h"
#include "timer.h"
#include "keypad.h"

#define KEYPAD_DEBOUNCE_MS  20

/* key at [row][col] */
static const char keypad_map[4][4] =
{
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' }
};

/* pins */
static char keypad_row_port;
static unsigned char keypad_row_pin;
static char keypad_col_port;
static unsigned char keypad_col_pin;

/* 1 after keypad_init worked */
static unsigned char keypad_ok = 0;

/* debounce state */
static char keypad_last_raw = KEYPAD_NO_KEY;
static char keypad_stable = KEYPAD_NO_KEY;
static unsigned long keypad_change_time = 0;

/* wait a few us, so the column pins settle after a row goes low */
static void keypad_wait_short(void)
{
    volatile unsigned char count;   /* volatile, so loop is kept */

    for (count = 0; count < 8; count++)
    {
    }
}

/* 1 if port letter and first pin of 4 are valid */
static unsigned char keypad_pins_ok(char port, unsigned char first_pin)
{
    if (port < 'A' || port > 'L' || port == 'I' || first_pin > 4)
    {
        return 0;
    }
    return 1;
}

/* set pins, starts timer, returns 1 ok or 0 wrong pins */
unsigned char keypad_init(char row_port, unsigned char row_first_pin,
                          char col_port, unsigned char col_first_pin)
{
    unsigned char i;

    if (!keypad_pins_ok(row_port, row_first_pin) || !keypad_pins_ok(col_port, col_first_pin))
    {
        return 0;
    }

    keypad_row_port = row_port;
    keypad_row_pin = row_first_pin;
    keypad_col_port = col_port;
    keypad_col_pin = col_first_pin;

    for (i = 0; i < 4; i++)
    {
        gpio_dir(row_port, row_first_pin + i, GPIO_OUT);
        gpio_set(row_port, row_first_pin + i);           /* rows idle high */
        gpio_dir(col_port, col_first_pin + i, GPIO_IN_PULLUP);
    }

    keypad_ok = 1;
    timer_init();   /* debounce needs timer_millis */

    return 1;
}

/* key held now (no debounce), or KEYPAD_NO_KEY */
char keypad_get_key(void)
{
    unsigned char row;
    unsigned char col;
    char key = KEYPAD_NO_KEY;

    if (!keypad_ok)
    {
        return KEYPAD_NO_KEY;
    }

    /* one row low at a time, a pressed key pulls its column low */
    for (row = 0; row < 4 && key == KEYPAD_NO_KEY; row++)
    {
        gpio_clear(keypad_row_port, keypad_row_pin + row);
        keypad_wait_short();

        for (col = 0; col < 4; col++)
        {
            if (gpio_get(keypad_col_port, keypad_col_pin + col) == 0)
            {
                key = keypad_map[row][col];
                break;
            }
        }

        gpio_set(keypad_row_port, keypad_row_pin + row);
    }

    return key;
}

/* key once per press (20 ms debounce), or KEYPAD_NO_KEY, call it often */
char keypad_was_pressed(void)
{
    char raw;

    if (!keypad_ok)
    {
        return KEYPAD_NO_KEY;
    }

    raw = keypad_get_key();

    if (raw != keypad_last_raw)
    {
        /* key changed, start the 20 ms wait again */
        keypad_last_raw = raw;
        keypad_change_time = timer_millis();
        return KEYPAD_NO_KEY;
    }

    /* same key for 20 ms and it is new */
    if (timer_elapsed(keypad_change_time, KEYPAD_DEBOUNCE_MS) && raw != keypad_stable)
    {
        keypad_stable = raw;
        return raw;   /* new key, or KEYPAD_NO_KEY on release */
    }

    return KEYPAD_NO_KEY;
}
