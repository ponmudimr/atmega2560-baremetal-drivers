/*
 * seg7.c - 2-digit 7-segment display, refreshed by Timer0
 * Author: Ponmudi
 */

#include "regs.h"
#include "board.h"
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

/* get digit pin for a position */
static unsigned char seg7_digit_pin(unsigned char pos)
{
    if (pos == 0)
    {
        return SEG7_DIGIT1_PIN;
    }
    else
    {
        return SEG7_DIGIT2_PIN;
    }
}

/* turn one digit on (1) or off (0) */
static void seg7_digit(unsigned char pos, unsigned char on)
{
    unsigned char level;

    if (on)
    {
        level = SEG7_DIGIT_ON_LEVEL;
    }
    else
    {
        level = !SEG7_DIGIT_ON_LEVEL;
    }

    if (level)
    {
        gpio_set(SEG7_DIGIT_PORT, seg7_digit_pin(pos));
    }
    else
    {
        gpio_clear(SEG7_DIGIT_PORT, seg7_digit_pin(pos));
    }
}

/* write 8 segment bits, 1 = segment on */
static void seg7_write_segments(unsigned char pattern)
{
    if (SEG7_COMMON_ANODE)
    {
        pattern = ~pattern;   /* anode: 0 lights segment */
    }

    gpio_port_write(SEG7_SEG_PORT, pattern);
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

/* set pins, start 2 ms refresh, interrupts on */
void seg7_init(void)
{
    gpio_port_dir(SEG7_SEG_PORT, 0xFF);        /* all 8 segment pins out */
    seg7_write_segments(0x00);

    gpio_dir(SEG7_DIGIT_PORT, SEG7_DIGIT1_PIN, GPIO_OUT);
    gpio_dir(SEG7_DIGIT_PORT, SEG7_DIGIT2_PIN, GPIO_OUT);
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
