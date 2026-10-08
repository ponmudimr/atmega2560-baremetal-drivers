/*
 * m2560_gpio.h - GPIO driver for ATmega2560
 * Author: Ponmudi
 * Author: Kavin
 */

#ifndef M2560_GPIO_H
#define M2560_GPIO_H

/* values for dir */
#define M2560_DIR_IN         0   /* input, floating */
#define M2560_DIR_OUT        1   /* output */
#define M2560_DIR_IN_PULLUP  2   /* input with pull-up */

/* port = 'A'..'L' (no 'I'), pin = 0..7 */

/* set pin as input, output or pull-up */
void m2560_gpio_dir(char port, unsigned char pin, unsigned char dir);

/* make pin high */
void m2560_gpio_set(char port, unsigned char pin);

/* make pin low */
void m2560_gpio_clear(char port, unsigned char pin);

/* read pin, returns 0 or 1 */
unsigned char m2560_gpio_get(char port, unsigned char pin);

/* flip pin */
void m2560_gpio_invert(char port, unsigned char pin);

#endif
