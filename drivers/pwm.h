/*
 * pwm.h - 2 kHz PWM for buzzer on OC4A (Timer4)
 * Author: Ponmudi
 */

#ifndef PWM_H
#define PWM_H

/* set pin, start Timer4, output stays off */
void pwm_init(void);

/* duty in percent, 0..100 */
void pwm_set_duty(unsigned char duty);

/* connect PWM to pin */
void pwm_on(void);

/* disconnect PWM, pin low */
void pwm_off(void);

#endif
