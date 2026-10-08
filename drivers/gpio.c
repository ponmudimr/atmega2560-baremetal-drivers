/*
 * gpio.c - GPIO driver for ATmega2560
 * Author: Ponmudi
 * Author: Pranesh
 * Author: Kavin
 */

#include "regs.h"
#include "gpio.h"

/* get DDR register for a port, 0 if wrong */
static volatile unsigned char *gpio_ddr_reg(char port)
{
    switch (port)
    {
        case 'A': return &M2560_DDRA;
        case 'C': return &M2560_DDRC;
        case 'E': return &M2560_DDRE;
        case 'G': return &M2560_DDRG;
        case 'H': return &M2560_DDRH;
        case 'L': return &M2560_DDRL;
        default:  return 0;       /* wrong port */
    }
}

/* get PORT register for a port, 0 if wrong */
static volatile unsigned char *gpio_port_reg(char port)
{
    switch (port)
    {
        case 'A': return &M2560_PORTA;
        case 'C': return &M2560_PORTC;
        case 'E': return &M2560_PORTE;
        case 'G': return &M2560_PORTG;
        case 'H': return &M2560_PORTH;
        case 'L': return &M2560_PORTL;
        default:  return 0;
    }
}

/* get PIN register for a port, 0 if wrong */
static volatile unsigned char *gpio_pin_reg(char port)
{
    switch (port)
    {
        case 'A': return &M2560_PINA;
        case 'C': return &M2560_PINC;
        case 'E': return &M2560_PINE;
        case 'G': return &M2560_PING;
        case 'H': return &M2560_PINH;
        case 'L': return &M2560_PINL;
        default:  return 0;
    }
}

/* set pin as input, output or pull-up */
void gpio_dir(char port, unsigned char pin, unsigned char dir)
{
    volatile unsigned char *ddr = gpio_ddr_reg(port);
    volatile unsigned char *out = gpio_port_reg(port);

    /* wrong port or pin, do nothing */
    if (ddr == 0 || out == 0 || pin > 7)
    {
        return;
    }

    if (dir == GPIO_OUT)
    {
        *ddr |= (1 << pin);    /* set pin as output */
    }
    else if (dir == GPIO_IN_PULLUP)
    {
        *ddr &= ~(1 << pin);   /* set pin as input */
        *out |= (1 << pin);    /* turn on pull-up */
    }
    else
    {
        *ddr &= ~(1 << pin);   /* set pin as input */
        *out &= ~(1 << pin);   /* pull-up off */
    }
}

/* make pin high */
void gpio_set(char port, unsigned char pin)
{
    volatile unsigned char *out = gpio_port_reg(port);

    if (out == 0 || pin > 7)
    {
        return;
    }

    *out |= (1 << pin);   /* pin high */
}

/* make pin low */
void gpio_clear(char port, unsigned char pin)
{
    volatile unsigned char *out = gpio_port_reg(port);

    if (out == 0 || pin > 7)
    {
        return;
    }

    *out &= ~(1 << pin);  /* pin low */
}

/* read pin, returns 1 or 0 (0 if wrong port/pin) */
unsigned char gpio_get(char port, unsigned char pin)
{
    volatile unsigned char *in = gpio_pin_reg(port);

    if (in == 0 || pin > 7)
    {
        return 0;
    }

    /* PIN reg has the real pin level */
    if (*in & (1 << pin))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
