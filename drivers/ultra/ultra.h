/*
 * ultra.h - ultrasonic sensor (HC-SR04 type), uses Timer5
 * Author: Ponmudi
 */

#ifndef ULTRA_H
#define ULTRA_H

#define ULTRA_NO_ECHO  999   /* returned on timeout */

/* set pins, start Timer5 */
void ultra_init(void);

/* measure distance in cm, 999 if no echo */
unsigned int ultra_get_cm(void);

#endif
