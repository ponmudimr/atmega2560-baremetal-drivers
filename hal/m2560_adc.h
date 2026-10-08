/*
 * File    : m2560_adc.h
 * About   : ADC driver for ATmega2560. 10-bit result (0..1023),
 *           reference AVcc (5V), channels 0..15 = pins A0..A15.
 * Registers used : ADMUX, ADCSRA, ADCSRB, ADCL, ADCH (DIDR0/DIDR2 explained)
 *           see datasheet: ADC - Analog to Digital Converter
 * Author  : Ponmudi
 */

#ifndef M2560_ADC_H
#define M2560_ADC_H

/*
 * m2560_adc_init - turn on the ADC with AVcc reference and clk/128
 * no parameters
 * returns nothing
 */
void m2560_adc_init(void);

/*
 * m2560_adc_sample - do one conversion on a channel and wait for it
 * channel : 0..15 (A0..A15)
 * returns 0..1023, or 0 for a wrong channel
 */
unsigned int m2560_adc_sample(unsigned char channel);

/*
 * m2560_adc_to_millivolts - change a raw ADC value to millivolts
 * raw : ADC value 0..1023
 * returns 0..5000 mV (assumes AVcc is exactly 5V)
 */
unsigned int m2560_adc_to_millivolts(unsigned int raw);

#endif
