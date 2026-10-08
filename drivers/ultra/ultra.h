/*
 * ultra.h - ultrasonic sensor (HC-SR04) driver, any pins, uses Timer5
 * Author: Ponmudi
 */

#ifndef ULTRA_H
#define ULTRA_H

#define ULTRA_MAX         4       /* max sensors */
#define ULTRA_NONE        255     /* ultra_init failed */
#define ULTRA_NO_ECHO     999     /* cm functions: no echo */
#define ULTRA_NO_ECHO_US  65535   /* ultra_get_us: no echo (999 us is a real echo) */

/* add a sensor, start Timer5, returns id or ULTRA_NONE */
unsigned char ultra_init(char trig_port, unsigned char trig_pin, char echo_port, unsigned char echo_pin);

/* echo pulse width in us, ULTRA_NO_ECHO_US on timeout or wrong id */
unsigned int ultra_get_us(unsigned char id);

/* distance in cm, ULTRA_NO_ECHO on timeout or wrong id */
unsigned int ultra_get_cm(unsigned char id);

/* average cm of n reads (n 1..8), 60 ms apart, ULTRA_NO_ECHO if all fail */
unsigned int ultra_get_cm_avg(unsigned char id, unsigned char n);

#endif
