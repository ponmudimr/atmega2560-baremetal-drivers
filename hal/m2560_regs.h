/*
 * File    : m2560_regs.h
 * About   : All ATmega2560 registers used in this project, written by
 *           their address from the datasheet. No avr-libc headers.
 *           Also the bit numbers we need inside those registers.
 * Registers used : PINx/DDRx/PORTx (A..L), SREG, Timer1, Timer4, ADC
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
 *   Registers from 0x60 up (and ports H..L at 0x100 up) only have
 *   one address, the one in brackets.
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

/* ---------- Status register (see datasheet: AVR Status Register) ---------- */

/* 0x3F is the I/O address, 0x5F is the memory address we need */
#define M2560_SREG   (*(volatile unsigned char *)0x5F)   /* datasheet p.13 */
#define M2560_BIT_I  7    /* global interrupt enable bit */

/* ---------- Timer1 (see datasheet: 16-bit Timer/Counter) ---------- */

#define M2560_TCCR1A (*(volatile unsigned char *)0x80)   /* datasheet p.154 */
#define M2560_TCCR1B (*(volatile unsigned char *)0x81)   /* datasheet p.156 */
#define M2560_TCNT1L (*(volatile unsigned char *)0x84)   /* datasheet p.158 */
#define M2560_TCNT1H (*(volatile unsigned char *)0x85)   /* datasheet p.158 */
#define M2560_OCR1AL (*(volatile unsigned char *)0x88)   /* datasheet p.159 */
#define M2560_OCR1AH (*(volatile unsigned char *)0x89)   /* datasheet p.159 */
#define M2560_TIMSK1 (*(volatile unsigned char *)0x6F)   /* datasheet p.161 */
#define M2560_TIFR1  (*(volatile unsigned char *)0x36)   /* datasheet p.162 */

/* TCCR1B bits */
#define M2560_BIT_WGM12   3   /* CTC mode (with WGM13..10 = 0100) */
#define M2560_BIT_CS11    1   /* clock select, CS11 + CS10 = clk/64 */
#define M2560_BIT_CS10    0

/* TIMSK1 bits */
#define M2560_BIT_OCIE1A  1   /* compare match A interrupt enable */

/* TIFR1 bits */
#define M2560_BIT_OCF1A   1   /* compare match A flag, write 1 to clear */

/* ---------- Timer4 (see datasheet: 16-bit Timer/Counter) ---------- */

#define M2560_TCCR4A (*(volatile unsigned char *)0xA0)   /* datasheet p.154 */
#define M2560_TCCR4B (*(volatile unsigned char *)0xA1)   /* datasheet p.156 */
#define M2560_OCR4AL (*(volatile unsigned char *)0xA8)   /* datasheet p.159 */
#define M2560_OCR4AH (*(volatile unsigned char *)0xA9)   /* datasheet p.159 */
#define M2560_OCR4BL (*(volatile unsigned char *)0xAA)   /* datasheet p.160 */
#define M2560_OCR4BH (*(volatile unsigned char *)0xAB)   /* datasheet p.160 */
#define M2560_OCR4CL (*(volatile unsigned char *)0xAC)   /* datasheet p.160 */
#define M2560_OCR4CH (*(volatile unsigned char *)0xAD)   /* datasheet p.160 */

/* TCCR4A bits */
#define M2560_BIT_COM4A1  7   /* OC4A non-inverting PWM */
#define M2560_BIT_COM4B1  5   /* OC4B non-inverting PWM */
#define M2560_BIT_COM4C1  3   /* OC4C non-inverting PWM */
#define M2560_BIT_WGM40   0   /* with WGM42 -> Fast PWM 8-bit */

/* TCCR4B bits */
#define M2560_BIT_WGM42   3
#define M2560_BIT_CS41    1   /* clock select, CS41 + CS40 = clk/64 */
#define M2560_BIT_CS40    0

/* ---------- ADC (see datasheet: Analog to Digital Converter) ---------- */

#define M2560_ADCL   (*(volatile unsigned char *)0x78)   /* datasheet p.286 */
#define M2560_ADCH   (*(volatile unsigned char *)0x79)   /* datasheet p.286 */
#define M2560_ADCSRA (*(volatile unsigned char *)0x7A)   /* datasheet p.285 */
#define M2560_ADCSRB (*(volatile unsigned char *)0x7B)   /* datasheet p.287 */
#define M2560_ADMUX  (*(volatile unsigned char *)0x7C)   /* datasheet p.281 */
#define M2560_DIDR2  (*(volatile unsigned char *)0x7D)   /* datasheet p.288 */
#define M2560_DIDR0  (*(volatile unsigned char *)0x7E)   /* datasheet p.287 */

/* ADMUX bits */
#define M2560_BIT_REFS0   6   /* reference = AVcc */

/* ADCSRB bits */
#define M2560_BIT_MUX5    3   /* 1 = channels 8..15 */

/* ADCSRA bits */
#define M2560_BIT_ADEN    7   /* ADC on */
#define M2560_BIT_ADSC    6   /* start conversion, goes 0 when done */
#define M2560_BIT_ADPS2   2   /* ADPS2..0 = 111 -> clk/128 */
#define M2560_BIT_ADPS1   1
#define M2560_BIT_ADPS0   0

#endif
