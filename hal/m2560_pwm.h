/*
 * File    : m2560_pwm.h
 * About   : PWM driver for ATmega2560 using Timer4.
 *           Fast PWM 8-bit, prescaler 64 -> 16 MHz / 64 / 256 = ~976 Hz.
 *           Channel 'A' = OC4A = PH3 = D6
 *           Channel 'B' = OC4B = PH4 = D7
 *           Channel 'C' = OC4C = PH5 = D8
 * Registers used : TCCR4A, TCCR4B, OCR4AH/L, OCR4BH/L, OCR4CH/L
 *           (DDRH/PORTH through the GPIO driver)
 *           see datasheet: 16-bit Timer/Counter, Fast PWM Mode
 * Author  : Ponmudi
 */

#ifndef M2560_PWM_H
#define M2560_PWM_H

/*
 * m2560_pwm_init - set up Timer4 and start PWM on one channel
 * channel : 'A', 'B' or 'C' (other letters are ignored)
 * returns nothing
 * duty starts at whatever OCR4x holds (0 after reset)
 */
void m2560_pwm_init(char channel);

/*
 * m2560_pwm_duty - change the duty cycle of a channel
 * channel : 'A', 'B' or 'C'
 * duty    : 0..255 (0 = almost off, 255 = fully on)
 * returns nothing
 */
void m2560_pwm_duty(char channel, unsigned char duty);

/*
 * m2560_pwm_stop - stop PWM on a channel and make the pin low
 * channel : 'A', 'B' or 'C'
 * returns nothing
 */
void m2560_pwm_stop(char channel);

#endif
