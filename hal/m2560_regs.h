/*
 * File    : m2560_regs.h
 * About   : All ATmega2560 registers used in this project, written by
 *           their address from the datasheet. No avr-libc headers.
 * Registers used : PINx/DDRx/PORTx (A..L)
 *           see datasheet: section 33 "Register Summary" (p.399-404)
 * Author  : Ponmudi
 *
 * IMPORTANT - two addresses for the same register:
 *   The Register Summary shows low registers like this:
 *       0x04 (0x24)  DDRB
 *   0x04 is the I/O address. It is only used by the IN and OUT
 *   assembly instructions.
 *   0x24 (the one in brackets) is the data space (memory) address.
 *   The I/O registers sit after the 32 CPU registers in memory,
 *   so data address = I/O address + 0x20.
 *   A C pointer reads normal memory, so it MUST use the data space
 *   address (0x24 for DDRB). Using 0x04 would hit CPU register r4.
 *   Ports H..L (0x100 up) only have one address, the one in
 *   brackets.
 *
 * Each register is a pointer to that address, made volatile so the
 * compiler really reads/writes it every time and does not optimize
 * it away.
 */

#ifndef M2560_REGS_H
#define M2560_REGS_H

/* ---------- GPIO ports (see datasheet: I/O-Ports, Register Description) ---------- */

/* Port A */
#define M2560_PINA   (*(volatile unsigned char *)0x20)   /* datasheet p.96 */
#define M2560_DDRA   (*(volatile unsigned char *)0x21)   /* datasheet p.96 */
#define M2560_PORTA  (*(volatile unsigned char *)0x22)   /* datasheet p.96 */

/* Port B */
#define M2560_PINB   (*(volatile unsigned char *)0x23)   /* datasheet p.96 */
#define M2560_DDRB   (*(volatile unsigned char *)0x24)   /* datasheet p.96 */
#define M2560_PORTB  (*(volatile unsigned char *)0x25)   /* datasheet p.96 */

/* Port C */
#define M2560_PINC   (*(volatile unsigned char *)0x26)   /* datasheet p.97 */
#define M2560_DDRC   (*(volatile unsigned char *)0x27)   /* datasheet p.97 */
#define M2560_PORTC  (*(volatile unsigned char *)0x28)   /* datasheet p.97 */

/* Port D */
#define M2560_PIND   (*(volatile unsigned char *)0x29)   /* datasheet p.97 */
#define M2560_DDRD   (*(volatile unsigned char *)0x2A)   /* datasheet p.97 */
#define M2560_PORTD  (*(volatile unsigned char *)0x2B)   /* datasheet p.97 */

/* Port E */
#define M2560_PINE   (*(volatile unsigned char *)0x2C)   /* datasheet p.98 */
#define M2560_DDRE   (*(volatile unsigned char *)0x2D)   /* datasheet p.97 */
#define M2560_PORTE  (*(volatile unsigned char *)0x2E)   /* datasheet p.97 */

/* Port F */
#define M2560_PINF   (*(volatile unsigned char *)0x2F)   /* datasheet p.98 */
#define M2560_DDRF   (*(volatile unsigned char *)0x30)   /* datasheet p.98 */
#define M2560_PORTF  (*(volatile unsigned char *)0x31)   /* datasheet p.97 */

/* Port G */
#define M2560_PING   (*(volatile unsigned char *)0x32)   /* datasheet p.98 */
#define M2560_DDRG   (*(volatile unsigned char *)0x33)   /* datasheet p.98 */
#define M2560_PORTG  (*(volatile unsigned char *)0x34)   /* datasheet p.98 */

/* Port H - extended I/O, only a memory address */
#define M2560_PINH   (*(volatile unsigned char *)0x100)  /* datasheet p.99 */
#define M2560_DDRH   (*(volatile unsigned char *)0x101)  /* datasheet p.99 */
#define M2560_PORTH  (*(volatile unsigned char *)0x102)  /* datasheet p.98 */

/* Port J (there is no port I) */
#define M2560_PINJ   (*(volatile unsigned char *)0x103)  /* datasheet p.99 */
#define M2560_DDRJ   (*(volatile unsigned char *)0x104)  /* datasheet p.99 */
#define M2560_PORTJ  (*(volatile unsigned char *)0x105)  /* datasheet p.99 */

/* Port K */
#define M2560_PINK   (*(volatile unsigned char *)0x106)  /* datasheet p.99 */
#define M2560_DDRK   (*(volatile unsigned char *)0x107)  /* datasheet p.99 */
#define M2560_PORTK  (*(volatile unsigned char *)0x108)  /* datasheet p.99 */

/* Port L */
#define M2560_PINL   (*(volatile unsigned char *)0x109)  /* datasheet p.100 */
#define M2560_DDRL   (*(volatile unsigned char *)0x10A)  /* datasheet p.100 */
#define M2560_PORTL  (*(volatile unsigned char *)0x10B)  /* datasheet p.100 */

#endif
