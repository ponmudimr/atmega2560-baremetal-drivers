/*
 * File    : m2560_gpio.h
 * About   : GPIO driver for ATmega2560 (Arduino Mega 2560).
 *           Lets us make a pin input or output, write it, read it
 *           and flip it, using the port letter and pin number.
 * Registers used : DDRx, PORTx, PINx  (x = A..L, no port I)
 *           see datasheet: I/O-Ports
 * Author  : Ponmudi
 */

#ifndef M2560_GPIO_H
#define M2560_GPIO_H

/* pin direction values for m2560_gpio_dir() */
#define M2560_DIR_IN         0   /* input, no pull-up (pin floats) */
#define M2560_DIR_OUT        1   /* output */
#define M2560_DIR_IN_PULLUP  2   /* input with internal pull-up on */

/*
 * m2560_gpio_dir - set a pin as input, output or input with pull-up
 * port : port letter 'A'..'L' (no 'I')
 * pin  : pin number 0..7
 * dir  : M2560_DIR_IN, M2560_DIR_OUT or M2560_DIR_IN_PULLUP
 * returns nothing
 */
void m2560_gpio_dir(char port, unsigned char pin, unsigned char dir);

/*
 * m2560_gpio_set - make an output pin high (5V)
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * returns nothing
 */
void m2560_gpio_set(char port, unsigned char pin);

/*
 * m2560_gpio_clear - make an output pin low (0V)
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * returns nothing
 */
void m2560_gpio_clear(char port, unsigned char pin);

/*
 * m2560_gpio_get - read the level on a pin
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * returns 1 if pin is high, 0 if low (also 0 for a wrong port/pin)
 */
unsigned char m2560_gpio_get(char port, unsigned char pin);

/*
 * m2560_gpio_invert - flip an output pin (high -> low, low -> high)
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * returns nothing
 */
void m2560_gpio_invert(char port, unsigned char pin);

#endif
