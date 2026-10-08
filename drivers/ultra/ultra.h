/*
 * ultra.h - ultrasonic sensor (HC-SR04), uses Timer5
 * Author: Ponmudi
 */

#ifndef ULTRA_H
#define ULTRA_H

#define ULTRA_NO_ECHO     999     /* cm functions: no echo */
#define ULTRA_NO_ECHO_US  65535   /* ultra_get_us: no echo (999 us is a real echo) */

/* set pins, start Timer5 */
void ultra_init(void);

/* echo pulse width in us, ULTRA_NO_ECHO_US on timeout */
unsigned int ultra_get_us(void);

/* distance in cm, ULTRA_NO_ECHO on timeout */
unsigned int ultra_get_cm(void);

/* average cm of n reads (n 1..8), 60 ms apart, ULTRA_NO_ECHO if all fail */
unsigned int ultra_get_cm_avg(unsigned char n);

#endif
