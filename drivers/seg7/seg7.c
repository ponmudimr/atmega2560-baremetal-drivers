/*
 * seg7.c - 2-digit 7-segment display driver, any pins, refreshed by Timer0
 * Author: Ponmudi
 */

#include "regs.h"
#include "gpio.h"
#include "seg7.h"

#define SEG7_DASH   16   /* table index for dash */
#define SEG7_BLANK  17   /* table index for blank */

/* segment bits: bit0 = a ... bit6 = g, bit7 = dp */
static const unsigned char seg7_table[18] =
{
    0x3F,   /* 0 */
    0x06,   /* 1 */
    0x5B,   /* 2 */
    0x4F,   /* 3 */
    0x66,   /* 4 */
    0x6D,   /* 5 */
    0x7D,   /* 6 */
    0x07,   /* 7 */
    0x7F,   /* 8 */
    0x6F,   /* 9 */
    0x77,   /* A */
    0x7C,   /* b */
    0x39,   /* C */
    0x5E,   /* d */
    0x79,   /* E */
    0x71,   /* F */
    0x40,   /* dash, only g */
    0x00    /* blank */
};

/* pattern each digit shows, [0] = left, [1] = right */
static volatile unsigned char seg7_pat[2] = {0x00, 0x00};

/* dot on/off for each digit */
static volatile unsigned char seg7_dp[2] = {0, 0};

/* digit shown now, 0 or 1 */
static volatile unsigned char seg7_pos = 0;

/* pins and options, saved by seg7_init */
static char seg7_seg_port;                 /* a..g,dp on pins 0..7 */
static char seg7_dig_port[2];              /* [0] = left, [1] = right */
static unsigned char seg7_dig_pin[2];
static unsigned char seg7_type;            /* SEG7_CATHODE or SEG7_ANODE */
static unsigned char seg7_on_level;        /* pin level that turns a digit on */

/* 1 if port letter and pin are valid */
static unsigned char seg7_pin_ok(char port, unsigned char pin)
{
    if (port < 'A' || port > 'L' || port == 'I' || pin > 7)
    {
        return 0;
    }
    return 1;
}

/* turn one digit on (1) or off (0) */
static void seg7_digit(unsigned char pos, unsigned char on)
{
    unsigned char level;

    if (on)
    {
        level = seg7_on_level;
    }
    else
    {
        level = !seg7_on_level;
    }

    if (level)
    {
        gpio_set(seg7_dig_port[pos], seg7_dig_pin[pos]);
    }
    else
    {
        gpio_clear(seg7_dig_port[pos], seg7_dig_pin[pos]);
    }
}

/* write 8 segment bits, 1 = segment on */
static void seg7_write_segments(unsigned char pattern)
{
    unsigned char bit;

    if (seg7_type == SEG7_ANODE)
    {
        pattern = ~pattern;   /* anode: 0 lights segment */
    }

    for (bit = 0; bit < 8; bit++)
    {
        if (pattern & (1 << bit))
        {
            gpio_set(seg7_seg_port, bit);
        }
        else
        {
            gpio_clear(seg7_seg_port, bit);
        }
    }
}

/* Timer0 compare A = vector No.22 (p.101), gcc counts from 0 -> 21 */
/* runs every 2 ms, shows the next digit */
void __vector_21(void) __attribute__((signal, used, externally_visible));
void __vector_21(void)
{
    unsigned char pattern;

    /* both digits off, no ghosting */
    seg7_digit(0, 0);
    seg7_digit(1, 0);

    pattern = seg7_pat[seg7_pos];
    if (seg7_dp[seg7_pos])
    {
        pattern |= 0x80;   /* dot is bit 7 */
    }
    seg7_write_segments(pattern);

    seg7_digit(seg7_pos, 1);

    /* next time the other digit */
    if (seg7_pos == 0)
    {
        seg7_pos = 1;
    }
    else
    {
        seg7_pos = 0;
    }
}

/* save pins, start 2 ms refresh, interrupts on (wrong pins = nothing) */
void seg7_init(char seg_port, char d1_port, unsigned char d1_pin,
               char d2_port, unsigned char d2_pin,
               unsigned char type, unsigned char digit_on_level)
{
    unsigned char bit;

    if (!seg7_pin_ok(seg_port, 0) || !seg7_pin_ok(d1_port, d1_pin) || !seg7_pin_ok(d2_port, d2_pin))
    {
        return;   /* wrong pins, display stays off */
    }

    seg7_seg_port = seg_port;
    seg7_dig_port[0] = d1_port;
    seg7_dig_pin[0] = d1_pin;
    seg7_dig_port[1] = d2_port;
    seg7_dig_pin[1] = d2_pin;
    seg7_type = type;
    seg7_on_level = digit_on_level;

    for (bit = 0; bit < 8; bit++)
    {
        gpio_dir(seg7_seg_port, bit, GPIO_OUT);   /* segment pins out */
    }
    seg7_write_segments(0x00);

    gpio_dir(d1_port, d1_pin, GPIO_OUT);
    gpio_dir(d2_port, d2_pin, GPIO_OUT);
    seg7_digit(0, 0);                          /* digits off */
    seg7_digit(1, 0);

    seg7_blank();

    /* 16 MHz / 256 = 62.5 kHz, 125 counts = 2 ms */
    M2560_TCCR0A = (1 << M2560_BIT_WGM01);     /* CTC mode */
    M2560_TCCR0B = 0;                          /* stopped for now */
    M2560_OCR0A = 124;
    M2560_TIMSK0 |= (1 << M2560_BIT_OCIE0A);   /* compare A interrupt on */
    M2560_TCCR0B = (1 << M2560_BIT_CS02);      /* /256, timer starts */

    M2560_SREG |= (1 << M2560_BIT_I);          /* interrupts on */
}

/* show 0..99, 0..9 uses right digit only, above 99 shows dash */
void seg7_show_number(unsigned char num)
{
    if (num > 99)
    {
        seg7_show_dash();
        return;
    }

    if (num < 10)
    {
        seg7_pat[0] = seg7_table[SEG7_BLANK];   /* tens off */
    }
    else
    {
        seg7_pat[0] = seg7_table[num / 10];
    }
    seg7_pat[1] = seg7_table[num % 10];
}

/* show 0..15 (0-9, A-F) on one digit */
void seg7_show_digit(unsigned char pos, unsigned char value)
{
    if (pos > 1 || value > 15)
    {
        return;   /* wrong pos or value */
    }

    seg7_pat[pos] = seg7_table[value];
}

/* show "--" */
void seg7_show_dash(void)
{
    seg7_pat[0] = seg7_table[SEG7_DASH];
    seg7_pat[1] = seg7_table[SEG7_DASH];
}

/* turn both digits off (dots too) */
void seg7_blank(void)
{
    seg7_pat[0] = seg7_table[SEG7_BLANK];
    seg7_pat[1] = seg7_table[SEG7_BLANK];
    seg7_dp[0] = 0;
    seg7_dp[1] = 0;
}

/* dot on (1) or off (0) for one digit */
void seg7_set_dp(unsigned char pos, unsigned char on)
{
    if (pos > 1)
    {
        return;
    }

    if (on)
    {
        seg7_dp[pos] = 1;
    }
    else
    {
        seg7_dp[pos] = 0;
    }
}

/* show own pattern on one digit, bit0 = a ... bit6 = g, bit7 = dp */
void seg7_show_raw(unsigned char pos, unsigned char pattern)
{
    if (pos > 1)
    {
        return;
    }

    seg7_pat[pos] = pattern;
}
