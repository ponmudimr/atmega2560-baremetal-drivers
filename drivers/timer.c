/*
 * timer.c - 1 ms tick on Timer1 (CTC, /64, OCR1A = 249)
 * Author: Ponmudi
 */

#include "regs.h"
#include "timer.h"

/* ms count, changed by the interrupt */
static volatile unsigned long timer_ticks = 0;

/* Timer1 compare A = vector No.18 (p.101), gcc counts from 0 -> 17 */
/* signal = save regs and return with RETI */
void __vector_17(void) __attribute__((signal, used, externally_visible));
void __vector_17(void)
{
    timer_ticks++;
}

/* start 1 ms tick, interrupts on */
void timer_init(void)
{
    M2560_TCCR1A = 0;                      /* no output pins */
    M2560_TCCR1B = 0;                      /* timer stopped for now */

    /* 16 MHz / 64 = 250 kHz, 250 counts = 1 ms */
    M2560_OCR1AH = 0;                      /* high byte first */
    M2560_OCR1AL = 249;

    M2560_TIMSK1 |= (1 << M2560_BIT_OCIE1A);   /* compare A interrupt on */

    M2560_TCCR1B |= (1 << M2560_BIT_WGM12);    /* CTC mode */
    M2560_TCCR1B |= (1 << M2560_BIT_CS11);     /* /64, timer starts */
    M2560_TCCR1B |= (1 << M2560_BIT_CS10);

    M2560_SREG |= (1 << M2560_BIT_I);          /* interrupts on */
}

/* ms since timer_init */
unsigned long timer_millis(void)
{
    unsigned long copy;
    unsigned char old_sreg;

    /* 4-byte copy is not atomic on 8-bit CPU */
    old_sreg = M2560_SREG;
    M2560_SREG &= ~(1 << M2560_BIT_I);     /* interrupts off */
    copy = timer_ticks;
    M2560_SREG = old_sreg;                 /* back as before */

    return copy;
}

/* wait ms milliseconds (needs timer_init) */
void timer_delay_ms(unsigned long ms)
{
    unsigned long start = timer_millis();

    /* subtract works after rollover too */
    while ((timer_millis() - start) < ms)
    {
    }
}
