/*
 * File    : m2560_adc.c
 * About   : ADC driver. Single conversion, wait for it (polling),
 *           no interrupts.
 *           ADC clock must be 50-200 kHz for full 10-bit accuracy.
 *           16 MHz / 128 = 125 kHz, so prescaler 128 is used.
 * Registers used : ADMUX, ADCSRA, ADCSRB, ADCL, ADCH
 *           see datasheet: ADC - Analog to Digital Converter
 * Author  : Ponmudi
 *
 * About DIDR0 / DIDR2 (Digital Input Disable):
 *   Each analog pin also has a digital input buffer. Setting its
 *   DIDR bit turns that buffer off and saves a little power when the
 *   pin has an analog voltage on it. It is not needed for the ADC to
 *   work, so this driver leaves DIDR0 / DIDR2 alone. Pins stay usable
 *   as normal GPIO too.
 */

#include "m2560_regs.h"
#include "m2560_adc.h"

/*
 * m2560_adc_init - turn on the ADC with AVcc reference and clk/128
 * no parameters
 * returns nothing
 */
void m2560_adc_init(void)
{
    /* REFS0 = 1, REFS1 = 0 -> reference is AVcc.
       ADLAR = 0 -> result is right adjusted (bits 9..0).
       MUX bits = 0 -> channel 0 for now. */
    M2560_ADMUX = (1 << M2560_BIT_REFS0);

    /* MUX5 = 0 -> channels 0..7 */
    M2560_ADCSRB &= ~(1 << M2560_BIT_MUX5);

    /* turn ADC on and set prescaler 128 (ADPS2..0 = 111) */
    M2560_ADCSRA = (1 << M2560_BIT_ADEN) | (1 << M2560_BIT_ADPS2) | (1 << M2560_BIT_ADPS1) | (1 << M2560_BIT_ADPS0);

    /* first conversion after turning on takes longer (25 ADC clocks)
       and may be off, so do one and throw the result away */
    m2560_adc_sample(0);
}

/*
 * m2560_adc_sample - do one conversion on a channel and wait for it
 * channel : 0..15 (A0..A15)
 * returns 0..1023, or 0 for a wrong channel
 */
unsigned int m2560_adc_sample(unsigned char channel)
{
    unsigned char low_byte;
    unsigned char high_byte;

    if (channel > 15)
    {
        return 0;
    }

    /* clear old channel: MUX4..0 are bits 4..0 of ADMUX (0x1F),
       and MUX5 is in ADCSRB */
    M2560_ADMUX &= ~(0x1F);
    M2560_ADCSRB &= ~(1 << M2560_BIT_MUX5);

    /*
     * The channel number needs 6 bits MUX5..MUX0, but MUX5 is not in
     * ADMUX with the others, it is bit 3 of ADCSRB.
     * Channels 0..7  : MUX5 = 0, MUX4..0 = channel
     * Channels 8..15 : MUX5 = 1, MUX4..0 = channel - 8
     * (see datasheet: Table 26-4, Input Channel Selections)
     */
    if (channel < 8)
    {
        M2560_ADMUX |= channel;
    }
    else
    {
        M2560_ADMUX |= (channel - 8);
        M2560_ADCSRB |= (1 << M2560_BIT_MUX5);   /* select upper channels A8..A15 */
    }

    M2560_ADCSRA |= (1 << M2560_BIT_ADSC);       /* start conversion */

    while (M2560_ADCSRA & (1 << M2560_BIT_ADSC))
    {
        /* ADSC stays 1 while converting, goes 0 when done */
    }

    /*
     * Read ADCL FIRST, then ADCH. Reading ADCL locks the result
     * registers so a new conversion can not change them. Reading ADCH
     * unlocks them. If ADCH is read first, the result stays locked and
     * later readings can be wrong or stuck.
     */
    low_byte = M2560_ADCL;
    high_byte = M2560_ADCH;

    return ((unsigned int)high_byte << 8) | low_byte;
}

/*
 * m2560_adc_to_millivolts - change a raw ADC value to millivolts
 * raw : ADC value 0..1023
 * returns 0..5000 mV (assumes AVcc is exactly 5V)
 */
unsigned int m2560_adc_to_millivolts(unsigned int raw)
{
    /* 1023 * 5000 = 5115000, too big for 16-bit unsigned int,
       so do the math in unsigned long (32-bit) */
    unsigned long mv = ((unsigned long)raw * 5000UL) / 1023UL;

    return (unsigned int)mv;
}
