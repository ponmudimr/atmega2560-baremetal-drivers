/*
 * gpio.h - GPIO driver for ATmega2560, ports A..L (no I)
 * Author: Ponmudi
 * Author: Kavin
 */

#ifndef GPIO_H
#define GPIO_H

/* values for dir */
#define GPIO_IN         0   /* input, floating */
#define GPIO_OUT        1   /* output */
#define GPIO_IN_PULLUP  2   /* input with pull-up */

/* port = 'A'..'L' (no 'I'), pin = 0..7, wrong values are ignored */

/* set pin as input, output or pull-up */
void gpio_dir(char port, unsigned char pin, unsigned char dir);

/* make pin high */
void gpio_set(char port, unsigned char pin);

/* make pin low */
void gpio_clear(char port, unsigned char pin);

/* read pin, returns 0 or 1 */
unsigned char gpio_get(char port, unsigned char pin);

/* flip pin */
void gpio_invert(char port, unsigned char pin);

/* set direction of whole port, bit 1 = output */
void gpio_port_dir(char port, unsigned char mask);

/* write whole port */
void gpio_port_write(char port, unsigned char value);

/* read whole port, 0 if wrong port */
unsigned char gpio_port_read(char port);

#endif
