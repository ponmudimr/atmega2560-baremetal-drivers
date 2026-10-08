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

/* status reg, I/O addr 0x3F */
#define M2560_SREG   (*(volatile unsigned char *)0x5F)   /* SREG, p.13 */
#define M2560_BIT_I       7   /* global interrupt on */

/* timer1 */
#define M2560_TCCR1A (*(volatile unsigned char *)0x80)   /* TCCR1A, p.154 */
#define M2560_TCCR1B (*(volatile unsigned char *)0x81)   /* TCCR1B, p.156 */
#define M2560_OCR1AL (*(volatile unsigned char *)0x88)   /* OCR1AL, p.159 */
#define M2560_OCR1AH (*(volatile unsigned char *)0x89)   /* OCR1AH, p.159 */
#define M2560_TIMSK1 (*(volatile unsigned char *)0x6F)   /* TIMSK1, p.161 */
#define M2560_BIT_WGM12   3   /* TCCR1B, CTC mode */
#define M2560_BIT_CS11    1   /* TCCR1B, clk/64 with CS10 */
#define M2560_BIT_CS10    0   /* TCCR1B */
#define M2560_BIT_OCIE1A  1   /* TIMSK1, compare A interrupt */

/* timer0 */
#define M2560_TCCR0A (*(volatile unsigned char *)0x44)   /* TCCR0A, p.126 */
#define M2560_TCCR0B (*(volatile unsigned char *)0x45)   /* TCCR0B, p.129 */
#define M2560_OCR0A  (*(volatile unsigned char *)0x47)   /* OCR0A, p.130 */
#define M2560_TIMSK0 (*(volatile unsigned char *)0x6E)   /* TIMSK0, p.131 */
#define M2560_BIT_WGM01   1   /* TCCR0A, CTC mode */
#define M2560_BIT_CS02    2   /* TCCR0B, clk/256 */
#define M2560_BIT_OCIE0A  1   /* TIMSK0, compare A interrupt */

/* timer5 */
#define M2560_TCCR5A (*(volatile unsigned char *)0x120)  /* TCCR5A, p.154 */
#define M2560_TCCR5B (*(volatile unsigned char *)0x121)  /* TCCR5B, p.156 */
#define M2560_TCNT5L (*(volatile unsigned char *)0x124)  /* TCNT5L, p.158 */
#define M2560_TCNT5H (*(volatile unsigned char *)0x125)  /* TCNT5H, p.158 */
#define M2560_BIT_CS51    1   /* TCCR5B, clk/8 */

/* timer4 */
#define M2560_TCCR4A (*(volatile unsigned char *)0xA0)   /* TCCR4A, p.154 */
#define M2560_TCCR4B (*(volatile unsigned char *)0xA1)   /* TCCR4B, p.156 */
#define M2560_ICR4L  (*(volatile unsigned char *)0xA6)   /* ICR4L, p.161 */
#define M2560_ICR4H  (*(volatile unsigned char *)0xA7)   /* ICR4H, p.161 */
#define M2560_OCR4AL (*(volatile unsigned char *)0xA8)   /* OCR4AL, p.159 */
#define M2560_OCR4AH (*(volatile unsigned char *)0xA9)   /* OCR4AH, p.159 */
#define M2560_BIT_COM4A1  7   /* TCCR4A, OC4A non-inverting */
#define M2560_BIT_WGM41   1   /* TCCR4A, mode 14 with WGM42, WGM43 */
#define M2560_BIT_WGM43   4   /* TCCR4B */
#define M2560_BIT_WGM42   3   /* TCCR4B */
#define M2560_BIT_CS41    1   /* TCCR4B, clk/8 */

/* ADC */
#define M2560_ADCL   (*(volatile unsigned char *)0x78)   /* ADCL, p.286 */
#define M2560_ADCH   (*(volatile unsigned char *)0x79)   /* ADCH, p.286 */
#define M2560_ADCSRA (*(volatile unsigned char *)0x7A)   /* ADCSRA, p.285 */
#define M2560_ADMUX  (*(volatile unsigned char *)0x7C)   /* ADMUX, p.281 */
#define M2560_BIT_REFS0   6   /* ADMUX, AVcc reference */
#define M2560_BIT_ADEN    7   /* ADCSRA, ADC on */
#define M2560_BIT_ADSC    6   /* ADCSRA, start, 0 when done */
#define M2560_BIT_ADPS2   2   /* ADCSRA, ADPS2..0 = 111 -> /128 */
#define M2560_BIT_ADPS1   1
#define M2560_BIT_ADPS0   0

#endif
