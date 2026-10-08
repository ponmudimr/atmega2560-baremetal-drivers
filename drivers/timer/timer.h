/*
 * timer.h - 1 ms tick on Timer1
 * Author: Ponmudi
 */

#ifndef TIMER_H
#define TIMER_H

/* start 1 ms tick, interrupts on */
void timer_init(void);

/* ms since timer_init */
unsigned long timer_millis(void);

/* wait ms milliseconds (needs timer_init) */
void timer_delay_ms(unsigned long ms);

#endif
