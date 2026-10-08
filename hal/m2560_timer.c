/*
 * File    : m2560_timer.c
 * About   : Timer1 in CTC mode gives an interrupt every 1 ms.
 *           Math: 16 MHz / 64 (prescaler) = 250 kHz -> 4 us per count.
 *           Count 0..249 = 250 counts = 1 ms, so OCR1A = 249.
 *           The interrupt only adds 1 to a millisecond counter.
 * Registers used : TCCR1A, TCCR1B, OCR1AH/L, TCNT1H/L, TIMSK1, TIFR1, SREG
 *           see datasheet: 16-bit Timer/Counter, Interrupts
 * Author  : Ponmudi
 */

#include "m2560_regs.h"
#include "m2560_timer.h"

/* milliseconds since start. volatile because the interrupt changes it */
static volatile unsigned long m2560_tick_count = 0;

/*
 * Interrupt handler for Timer1 compare match A.
 * Datasheet Table 14-1 (p.101) lists it as vector No. 18.
 * The datasheet starts counting at 1 (No. 1 = RESET), but avr-gcc
 * starts at 0 (reset is vector 0), so No. 18 becomes __vector_17.
 * The vector table in the startup code jumps to this name.
 * "signal" tells the compiler this is an interrupt: it saves every
 * register it uses (and SREG) and returns with RETI, not RET.
 * "used" and "externally_visible" stop the compiler from removing it,
 * because nobody calls it from C code.
 */
void __vector_17(void) __attribute__((signal, used, externally_visible));
void __vector_17(void)
{
    m2560_tick_count++;
}

/*
 * m2560_timer_init - start Timer1 with a 1 ms interrupt and turn on
 *                    global interrupts
 * no parameters
 * returns nothing
 */
void m2560_timer_init(void)
{
    M2560_TCCR1A = 0;    /* WGM11, WGM10 = 0, no output pins used */
    M2560_TCCR1B = 0;    /* stop timer while we set it up */

    /* 16-bit write: high byte first, then low byte (shared TEMP register) */
    M2560_TCNT1H = 0;    /* start counting from 0 */
    M2560_TCNT1L = 0;
    M2560_OCR1AH = 0;    /* 249 = 0x00F9, high byte is 0 */
    M2560_OCR1AL = 249;  /* compare value for 1 ms */

    M2560_TIFR1 = (1 << M2560_BIT_OCF1A);     /* clear old compare flag (writing 1 clears it) */
    M2560_TIMSK1 |= (1 << M2560_BIT_OCIE1A);  /* enable compare match A interrupt */

    M2560_TCCR1B |= (1 << M2560_BIT_WGM12);   /* CTC mode, timer resets at OCR1A */
    M2560_TCCR1B |= (1 << M2560_BIT_CS11);    /* prescaler 64 ... */
    M2560_TCCR1B |= (1 << M2560_BIT_CS10);    /* ... CS11 + CS10, timer starts now */

    M2560_SREG |= (1 << M2560_BIT_I);         /* global interrupts on (same as sei) */
}

/*
 * m2560_timer_millis - get milliseconds since m2560_timer_init()
 * no parameters
 * returns the millisecond count (goes back to 0 after about 49 days)
 */
unsigned long m2560_timer_millis(void)
{
    unsigned long copy;
    unsigned char old_sreg;

    /*
     * The counter is 32-bit but the CPU is 8-bit, so copying it takes
     * 4 separate byte reads. If the interrupt comes in the middle we
     * get half old and half new bytes (a wrong value). So we turn off
     * interrupts while copying. We save SREG first and put it back
     * after, so if interrupts were off before, they stay off.
     */
    old_sreg = M2560_SREG;                /* remember interrupt flag */
    M2560_SREG &= ~(1 << M2560_BIT_I);    /* interrupts off (same as cli) */
    copy = m2560_tick_count;
    M2560_SREG = old_sreg;                /* put interrupt flag back */

    return copy;
}

/*
 * m2560_timer_wait_ms - wait here for some milliseconds
 * ms : how many milliseconds to wait
 * returns nothing
 * needs m2560_timer_init() first, or it waits forever
 */
void m2560_timer_wait_ms(unsigned long ms)
{
    unsigned long start = m2560_timer_millis();

    /* subtracting still works when the counter rolls over to 0 */
    while ((m2560_timer_millis() - start) < ms)
    {
        /* just wait */
    }
}
