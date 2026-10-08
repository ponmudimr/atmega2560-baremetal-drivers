/*
 * ultra.c - ultrasonic sensor (HC-SR04), uses Timer5
 * Author: Ponmudi
 */

#include "regs.h"
#include "board.h"
#include "gpio.h"
#include "ultra.h"

/* Timer5 /8 -> 1 tick = 0.5 us */
#define ULTRA_TRIG_TICKS    20      /* 10 us */
#define ULTRA_TIMEOUT_TICKS 60000   /* 30 ms */

/* set Timer5 count to 0 */
static void ultra_timer_clear(void)
{
    M2560_TCNT5H = 0;   /* high byte first */
    M2560_TCNT5L = 0;
}

/* read Timer5 count */
static unsigned int ultra_timer_read(void)
{
    unsigned char low_byte;
    unsigned char high_byte;

    low_byte = M2560_TCNT5L;    /* low byte first */
    high_byte = M2560_TCNT5H;

    return ((unsigned int)high_byte << 8) | low_byte;
}

/* wait some Timer5 ticks (max 65535 = 32 ms) */
static void ultra_wait_ticks(unsigned int ticks)
{
    ultra_timer_clear();
    while (ultra_timer_read() < ticks)
    {
    }
}

/* set pins, start Timer5 */
void ultra_init(void)
{
    gpio_dir(ULTRA_PORT, ULTRA_TRIG_PIN, GPIO_OUT);
    gpio_clear(ULTRA_PORT, ULTRA_TRIG_PIN);         /* trig low */
    gpio_dir(ULTRA_PORT, ULTRA_ECHO_PIN, GPIO_IN);

    M2560_TCCR5A = 0;                         /* normal mode */
    M2560_TCCR5B = (1 << M2560_BIT_CS51);     /* /8, timer runs */
}

/* echo pulse width in us, ULTRA_NO_ECHO_US on timeout */
unsigned int ultra_get_us(void)
{
    unsigned int ticks;

    /* 10 us trigger pulse */
    gpio_set(ULTRA_PORT, ULTRA_TRIG_PIN);
    ultra_wait_ticks(ULTRA_TRIG_TICKS);
    gpio_clear(ULTRA_PORT, ULTRA_TRIG_PIN);

    /* wait echo high */
    ultra_timer_clear();
    while (gpio_get(ULTRA_PORT, ULTRA_ECHO_PIN) == 0)
    {
        if (ultra_timer_read() >= ULTRA_TIMEOUT_TICKS)
        {
            return ULTRA_NO_ECHO_US;
        }
    }

    /* measure how long echo stays high */
    ultra_timer_clear();
    while (gpio_get(ULTRA_PORT, ULTRA_ECHO_PIN) == 1)
    {
        if (ultra_timer_read() >= ULTRA_TIMEOUT_TICKS)
        {
            return ULTRA_NO_ECHO_US;
        }
    }
    ticks = ultra_timer_read();

    return ticks / 2;   /* 2 ticks = 1 us */
}

/* distance in cm, ULTRA_NO_ECHO on timeout */
unsigned int ultra_get_cm(void)
{
    unsigned int us = ultra_get_us();

    if (us == ULTRA_NO_ECHO_US)
    {
        return ULTRA_NO_ECHO;
    }

    /* sound: 58 us per cm, there and back */
    return us / 58;
}

/* average cm of n reads (n 1..8), 60 ms apart, ULTRA_NO_ECHO if all fail */
unsigned int ultra_get_cm_avg(unsigned char n)
{
    unsigned long sum = 0;
    unsigned char good = 0;
    unsigned char i;
    unsigned int cm;

    if (n < 1 || n > 8)
    {
        return ULTRA_NO_ECHO;   /* wrong n */
    }

    for (i = 0; i < n; i++)
    {
        cm = ultra_get_cm();
        if (cm != ULTRA_NO_ECHO)
        {
            sum = sum + cm;   /* skip timeouts */
            good++;
        }

        if (i < n - 1)
        {
            /* 60 ms so old echoes die out */
            ultra_wait_ticks(ULTRA_TIMEOUT_TICKS);   /* 30 ms */
            ultra_wait_ticks(ULTRA_TIMEOUT_TICKS);   /* 30 ms */
        }
    }

    if (good == 0)
    {
        return ULTRA_NO_ECHO;
    }

    return (unsigned int)(sum / good);
}
