/*
 * pwm.h - PWM on Timer4, channels 'A' (D6), 'B' (D7), 'C' (D8)
 * Author: Ponmudi
 */

#ifndef PWM_H
#define PWM_H

/* set channel pin as output, start Timer4 once (2 kHz), output off */
void pwm_init(char ch);

/* frequency 31..65535 Hz, same for all 3 channels */
void pwm_set_freq(unsigned int hz);

/* duty 0..100 percent for one channel */
void pwm_set_duty(char ch, unsigned char duty);

/* connect PWM to the channel pin */
void pwm_on(char ch);

/* disconnect PWM, pin low */
void pwm_off(char ch);

#endif
