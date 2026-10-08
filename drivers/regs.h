/*
 * regs.h - ATmega2560 registers by address
 * Author: Ponmudi
 */

/*
 * Datasheet shows "0x04 (0x24) DDRB".
 * 0x04 is for IN/OUT, 0x24 is the memory address.
 * C pointers need the memory one (in brackets).
 */

#ifndef M2560_REGS_H
#define M2560_REGS_H

/* port A */
#define M2560_PINA   (*(volatile unsigned char *)0x20)   /* PINA, p.96 */
#define M2560_DDRA   (*(volatile unsigned char *)0x21)   /* DDRA, p.96 */
#define M2560_PORTA  (*(volatile unsigned char *)0x22)   /* PORTA, p.96 */

/* port C */
#define M2560_PINC   (*(volatile unsigned char *)0x26)   /* PINC, p.97 */
#define M2560_DDRC   (*(volatile unsigned char *)0x27)   /* DDRC, p.97 */
#define M2560_PORTC  (*(volatile unsigned char *)0x28)   /* PORTC, p.97 */

/* port E */
#define M2560_PINE   (*(volatile unsigned char *)0x2C)   /* PINE, p.98 */
#define M2560_DDRE   (*(volatile unsigned char *)0x2D)   /* DDRE, p.97 */
#define M2560_PORTE  (*(volatile unsigned char *)0x2E)   /* PORTE, p.97 */

/* port G */
#define M2560_PING   (*(volatile unsigned char *)0x32)   /* PING, p.98 */
#define M2560_DDRG   (*(volatile unsigned char *)0x33)   /* DDRG, p.98 */
#define M2560_PORTG  (*(volatile unsigned char *)0x34)   /* PORTG, p.98 */

/* port H */
#define M2560_PINH   (*(volatile unsigned char *)0x100)   /* PINH, p.99 */
#define M2560_DDRH   (*(volatile unsigned char *)0x101)   /* DDRH, p.99 */
#define M2560_PORTH  (*(volatile unsigned char *)0x102)   /* PORTH, p.98 */

/* port L */
#define M2560_PINL   (*(volatile unsigned char *)0x109)   /* PINL, p.100 */
#define M2560_DDRL   (*(volatile unsigned char *)0x10A)   /* DDRL, p.100 */
#define M2560_PORTL  (*(volatile unsigned char *)0x10B)   /* PORTL, p.100 */

#endif
