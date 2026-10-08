/*
 * adc.c - 10-bit ADC, AVcc reference
 * Author: Ponmudi
 */

#include "regs.h"
#include "adc.h"

/* ADC on, AVcc ref, clk/128 */
void adc_init(void)
{
    M2560_ADMUX = (1 << M2560_BIT_REFS0);      /* AVcc ref, right adjust, ch 0 */
    M2560_ADCSRB &= ~(1 << M2560_BIT_MUX5);

    /* 16 MHz / 128 = 125 kHz, inside 50..200 kHz */
    M2560_ADCSRA = (1 << M2560_BIT_ADEN) | (1 << M2560_BIT_ADPS2) | (1 << M2560_BIT_ADPS1) | (1 << M2560_BIT_ADPS0);

    adc_read(0);   /* first result is slow, throw away */
}

/* read channel 0..15, returns 0..1023 (0 if wrong channel) */
unsigned int adc_read(unsigned char channel)
{
    unsigned char low_byte;
    unsigned char high_byte;

    if (channel > 15)
    {
        return 0;
    }

    /* clear old channel, MUX4..0 = bits 4..0 */
    M2560_ADMUX &= ~(0x1F);
    M2560_ADCSRB &= ~(1 << M2560_BIT_MUX5);

    /* MUX5 is in ADCSRB, needed for 8..15 */
    if (channel < 8)
    {
        M2560_ADMUX |= channel;
    }
    else
    {
        M2560_ADMUX |= (channel - 8);
        M2560_ADCSRB |= (1 << M2560_BIT_MUX5);
    }

    M2560_ADCSRA |= (1 << M2560_BIT_ADSC);     /* start */
    while (M2560_ADCSRA & (1 << M2560_BIT_ADSC))
    {
        /* wait till done */
    }

    /* ADCL first, it locks the result till ADCH is read */
    low_byte = M2560_ADCL;
    high_byte = M2560_ADCH;

    return ((unsigned int)high_byte << 8) | low_byte;
}
