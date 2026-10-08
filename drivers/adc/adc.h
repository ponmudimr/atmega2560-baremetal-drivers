/*
 * adc.h - 10-bit ADC, AVcc (5V) reference, channels 0..15 (A0..A15)
 * Author: Ponmudi
 */

#ifndef ADC_H
#define ADC_H

/* ADC on, AVcc ref, clk/128 */
void adc_init(void);

/* read channel 0..15, returns 0..1023 (0 if wrong channel) */
unsigned int adc_read(unsigned char channel);

/* average of n reads (n 1..16), 0 if wrong channel or n */
unsigned int adc_read_avg(unsigned char channel, unsigned char n);

/* raw 0..1023 to millivolts 0..5000 */
unsigned int adc_to_mv(unsigned int raw);

#endif
