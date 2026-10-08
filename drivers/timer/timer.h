/*
 * timer.h - 1 ms tick on Timer1
 * Author: Ponmudi
 */

#ifndef TIMER_H
#define TIMER_H

/* start 1 ms tick, interrupts on (safe to call again) */
void timer_init(void);

/* ms since timer_init */
unsigned long timer_millis(void);

/* wait ms milliseconds (needs timer_init) */
void timer_delay_ms(unsigned long ms);

/* 1 if ms milliseconds passed since start (start = old timer_millis) */
unsigned char timer_elapsed(unsigned long start, unsigned long ms);

#endif
