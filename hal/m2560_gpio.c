/*
 * File    : m2560_gpio.c
 * About   : GPIO driver for ATmega2560 (Arduino Mega 2560).
 *           Every port has 3 registers:
 *             DDRx  - direction (1 = output, 0 = input)
 *             PORTx - output level, or pull-up on/off when pin is input
 *             PINx  - the real level on the pin (read only for us)
 *           Ports A..G sit in I/O space, H..L sit in extended I/O
 *           space, but avr/io.h handles that for us.
 * Registers used : DDRA..DDRL, PORTA..PORTL, PINA..PINL
 *           see datasheet: I/O-Ports, Register Description
 * Author  : Ponmudi
 * Author  : Pranesh
 */

#include <avr/io.h>
#include "m2560_gpio.h"

/*
 * m2560_ddr_reg - find the DDR register for a port letter
 * port : port letter 'A'..'L'
 * returns pointer to DDRx, or 0 if the letter is wrong
 */
static volatile unsigned char *m2560_ddr_reg(char port)
{
    switch (port)
    {
        case 'A': return &DDRA;
        case 'B': return &DDRB;
        case 'C': return &DDRC;
        case 'D': return &DDRD;
        case 'E': return &DDRE;
        case 'F': return &DDRF;
        case 'G': return &DDRG;
        case 'H': return &DDRH;
        case 'J': return &DDRJ;   /* no port I on this chip */
        case 'K': return &DDRK;
        case 'L': return &DDRL;
        default:  return 0;       /* wrong letter */
    }
}

/*
 * m2560_port_reg - find the PORT register for a port letter
 * port : port letter 'A'..'L'
 * returns pointer to PORTx, or 0 if the letter is wrong
 */
static volatile unsigned char *m2560_port_reg(char port)
{
    switch (port)
    {
        case 'A': return &PORTA;
        case 'B': return &PORTB;
        case 'C': return &PORTC;
        case 'D': return &PORTD;
        case 'E': return &PORTE;
        case 'F': return &PORTF;
        case 'G': return &PORTG;
        case 'H': return &PORTH;
        case 'J': return &PORTJ;
        case 'K': return &PORTK;
        case 'L': return &PORTL;
        default:  return 0;
    }
}

/*
 * m2560_pin_reg - find the PIN register for a port letter
 * port : port letter 'A'..'L'
 * returns pointer to PINx, or 0 if the letter is wrong
 */
static volatile unsigned char *m2560_pin_reg(char port)
{
    switch (port)
    {
        case 'A': return &PINA;
        case 'B': return &PINB;
        case 'C': return &PINC;
        case 'D': return &PIND;
        case 'E': return &PINE;
        case 'F': return &PINF;
        case 'G': return &PING;
        case 'H': return &PINH;
        case 'J': return &PINJ;
        case 'K': return &PINK;
        case 'L': return &PINL;
        default:  return 0;
    }
}

/*
 * m2560_gpio_dir - set a pin as input, output or input with pull-up
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * dir  : M2560_DIR_IN, M2560_DIR_OUT or M2560_DIR_IN_PULLUP
 * returns nothing
 */
void m2560_gpio_dir(char port, unsigned char pin, unsigned char dir)
{
    volatile unsigned char *ddr = m2560_ddr_reg(port);
    volatile unsigned char *out = m2560_port_reg(port);

    /* stop here if port letter or pin number is wrong */
    if (ddr == 0 || out == 0 || pin > 7)
    {
        return;
    }

    if (dir == M2560_DIR_OUT)
    {
        *ddr |= (1 << pin);    /* set bit in DDR so pin becomes output */
    }
    else if (dir == M2560_DIR_IN_PULLUP)
    {
        *ddr &= ~(1 << pin);   /* clear bit in DDR so pin becomes input */
        *out |= (1 << pin);    /* PORT bit = 1 on an input turns pull-up on */
    }
    else
    {
        *ddr &= ~(1 << pin);   /* clear bit in DDR so pin becomes input */
        *out &= ~(1 << pin);   /* PORT bit = 0 so pull-up is off (floating) */
    }
}

/*
 * m2560_gpio_set - make an output pin high (5V)
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * returns nothing
 */
void m2560_gpio_set(char port, unsigned char pin)
{
    volatile unsigned char *out = m2560_port_reg(port);

    if (out == 0 || pin > 7)
    {
        return;
    }

    *out |= (1 << pin);   /* set bit in PORT so pin goes high */
}

/*
 * m2560_gpio_clear - make an output pin low (0V)
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * returns nothing
 */
void m2560_gpio_clear(char port, unsigned char pin)
{
    volatile unsigned char *out = m2560_port_reg(port);

    if (out == 0 || pin > 7)
    {
        return;
    }

    *out &= ~(1 << pin);  /* clear bit in PORT so pin goes low */
}

/*
 * m2560_gpio_get - read the level on a pin
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * returns 1 if pin is high, 0 if low (also 0 for a wrong port/pin)
 */
unsigned char m2560_gpio_get(char port, unsigned char pin)
{
    volatile unsigned char *in = m2560_pin_reg(port);

    if (in == 0 || pin > 7)
    {
        return 0;
    }

    /* PIN register shows the real voltage level on the pin */
    if (*in & (1 << pin))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 * m2560_gpio_invert - flip an output pin (high -> low, low -> high)
 * port : port letter 'A'..'L'
 * pin  : pin number 0..7
 * returns nothing
 */
void m2560_gpio_invert(char port, unsigned char pin)
{
    volatile unsigned char *out = m2560_port_reg(port);

    if (out == 0 || pin > 7)
    {
        return;
    }

    *out ^= (1 << pin);   /* XOR flips only this bit in PORT */
}
