/*
 * led.h - LED driver for the 5 status LEDs
 * Author: Ponmudi
 */

#ifndef LED_H
#define LED_H

/* LED ids */
#define LED_SAFE      0
#define LED_CAUTION   1
#define LED_WARNING   2
#define LED_STOP      3
#define LED_OCCUPIED  4

/* set all LED pins as output, all off */
void led_init(void);

/* turn one LED on */
void led_on(unsigned char id);

/* turn one LED off */
void led_off(unsigned char id);

#endif
