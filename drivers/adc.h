/*
 * adc.h - 10-bit ADC, AVcc reference
 * Author: Ponmudi
 */

#ifndef ADC_H
#define ADC_H

/* ADC on, AVcc ref, clk/128 */
void adc_init(void);

/* read channel 0..7, returns 0..1023 (0 if wrong channel) */
unsigned int adc_read(unsigned char channel);

#endif
