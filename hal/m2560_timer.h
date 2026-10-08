/*
 * File    : m2560_timer.h
 * About   : Timer driver for ATmega2560. Timer1 makes a 1 ms tick
 *           and we count the ticks to get milliseconds since start.
 *           Timer4 is NOT used here, the PWM driver needs it.
 * Registers used : TCCR1A, TCCR1B, OCR1AH/L, TCNT1H/L, TIMSK1, TIFR1, SREG
 *           see datasheet: 16-bit Timer/Counter (Timer/Counter 1, 3, 4, and 5)
 * Author  : Ponmudi
 */

#ifndef M2560_TIMER_H
#define M2560_TIMER_H

/*
 * m2560_timer_init - start Timer1 with a 1 ms interrupt and turn on
 *                    global interrupts
 * no parameters
 * returns nothing
 */
void m2560_timer_init(void);

/*
 * m2560_timer_millis - get milliseconds since m2560_timer_init()
 * no parameters
 * returns the millisecond count (goes back to 0 after about 49 days)
 */
unsigned long m2560_timer_millis(void);

/*
 * m2560_timer_wait_ms - wait here for some milliseconds
 * ms : how many milliseconds to wait
 * returns nothing
 * needs m2560_timer_init() first, or it waits forever
 */
void m2560_timer_wait_ms(unsigned long ms);

#endif
