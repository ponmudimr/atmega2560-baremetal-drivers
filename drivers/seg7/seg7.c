/*
 * seg7.c - 2-digit common cathode 7-seg, refreshed by Timer0
 * Author: Ponmudi
 */

#include "regs.h"
#include "board.h"
#include "gpio.h"
#include "seg7.h"

#define SEG7_DASH   10   /* table index for dash */
#define SEG7_BLANK  11   /* table index for blank */

/* segment bits: bit0 = a ... bit6 = g, bit7 = dp */
static const unsigned char seg7_table[12] =
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
    0x40,   /* dash, only g */
    0x00    /* blank */
};

/* what each digit shows, [0] = tens, [1] = ones */
static volatile unsigned char seg7_buf[2] = {SEG7_BLANK, SEG7_BLANK};

/* digit shown now, 0 or 1 */
static volatile unsigned char seg7_pos = 0;

/* turn one digit on (1) or off (0), HIGH = on */
static void seg7_digit(unsigned char pin, unsigned char on)
{
    if (on)
    {
        gpio_set(SEG7_DIGIT_PORT, pin);
    }
    else
    {
        gpio_clear(SEG7_DIGIT_PORT, pin);
    }
}

/* write 8 segment bits to port, common cathode: 1 = on */
static void seg7_write_segments(unsigned char pattern)
{
    unsigned char bit;

    for (bit = 0; bit < 8; bit++)
    {
        if (pattern & (1 << bit))
        {
            gpio_set(SEG7_SEG_PORT, bit);
        }
        else
        {
            gpio_clear(SEG7_SEG_PORT, bit);
        }
    }
}

/* Timer0 compare A = vector No.22 (p.101), gcc counts from 0 -> 21 */
/* runs every 2 ms, shows the next digit */
void __vector_21(void) __attribute__((signal, used, externally_visible));
void __vector_21(void)
{
    /* both digits off, no ghosting */
    seg7_digit(SEG7_DIGIT1_PIN, 0);
    seg7_digit(SEG7_DIGIT2_PIN, 0);

    seg7_write_segments(seg7_table[seg7_buf[seg7_pos]]);

    if (seg7_pos == 0)
    {
        seg7_digit(SEG7_DIGIT1_PIN, 1);
        seg7_pos = 1;
    }
    else
    {
        seg7_digit(SEG7_DIGIT2_PIN, 1);
        seg7_pos = 0;
    }
}

/* set pins, start 2 ms refresh, interrupts on */
void seg7_init(void)
{
    unsigned char bit;

    for (bit = 0; bit < 8; bit++)
    {
        gpio_dir(SEG7_SEG_PORT, bit, GPIO_OUT);   /* segment pins out */
    }
    seg7_write_segments(0x00);

    gpio_dir(SEG7_DIGIT_PORT, SEG7_DIGIT1_PIN, GPIO_OUT);
    gpio_dir(SEG7_DIGIT_PORT, SEG7_DIGIT2_PIN, GPIO_OUT);
    seg7_digit(SEG7_DIGIT1_PIN, 0);   /* digits off */
    seg7_digit(SEG7_DIGIT2_PIN, 0);

    /* 16 MHz / 256 = 62.5 kHz, 125 counts = 2 ms */
    M2560_TCCR0A = (1 << M2560_BIT_WGM01);     /* CTC mode */
    M2560_TCCR0B = 0;                          /* stopped for now */
    M2560_OCR0A = 124;
    M2560_TIMSK0 |= (1 << M2560_BIT_OCIE0A);   /* compare A interrupt on */
    M2560_TCCR0B = (1 << M2560_BIT_CS02);      /* /256, timer starts */

    M2560_SREG |= (1 << M2560_BIT_I);          /* interrupts on */
}

/* show 0..99, 0..9 uses one digit, above 99 shows dash */
void seg7_show_number(unsigned char num)
{
    if (num > 99)
    {
        seg7_show_dash();
        return;
    }

    if (num < 10)
    {
        seg7_buf[0] = SEG7_BLANK;   /* tens off for one digit */
    }
    else
    {
        seg7_buf[0] = num / 10;
    }
    seg7_buf[1] = num % 10;
}

/* show "--" */
void seg7_show_dash(void)
{
    seg7_buf[0] = SEG7_DASH;
    seg7_buf[1] = SEG7_DASH;
}
