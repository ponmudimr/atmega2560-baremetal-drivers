/*
 * ultra.c - ultrasonic sensor (HC-SR04) driver, any pins, uses Timer5
 * Author: Ponmudi
 */

#include "regs.h"
#include "gpio.h"
#include "ultra.h"

/* Timer5 /8 -> 1 tick = 0.5 us */
#define ULTRA_TRIG_TICKS    20      /* 10 us */
#define ULTRA_TIMEOUT_TICKS 60000   /* 30 ms */

/* pins of each added sensor */
static char ultra_trig_port[ULTRA_MAX];
static unsigned char ultra_trig_pin[ULTRA_MAX];
static char ultra_echo_port[ULTRA_MAX];
static unsigned char ultra_echo_pin[ULTRA_MAX];

/* how many sensors are added */
static unsigned char ultra_count = 0;

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

/* 1 if port letter and pin are valid */
static unsigned char ultra_pin_ok(char port, unsigned char pin)
{
    if (port < 'A' || port > 'L' || port == 'I' || pin > 7)
    {
        return 0;
    }
    return 1;
}

/* add a sensor, start Timer5, returns id or ULTRA_NONE */
unsigned char ultra_init(char trig_port, unsigned char trig_pin, char echo_port, unsigned char echo_pin)
{
    unsigned char id;

    /* wrong pins or table full */
    if (!ultra_pin_ok(trig_port, trig_pin) || !ultra_pin_ok(echo_port, echo_pin) || ultra_count >= ULTRA_MAX)
    {
        return ULTRA_NONE;
    }

    id = ultra_count;
    ultra_trig_port[id] = trig_port;
    ultra_trig_pin[id] = trig_pin;
    ultra_echo_port[id] = echo_port;
    ultra_echo_pin[id] = echo_pin;
    ultra_count++;

    gpio_dir(trig_port, trig_pin, GPIO_OUT);
    gpio_clear(trig_port, trig_pin);          /* trig low */
    gpio_dir(echo_port, echo_pin, GPIO_IN);

    /* same setup for every sensor, safe to repeat */
    M2560_TCCR5A = 0;                         /* normal mode */
    M2560_TCCR5B = (1 << M2560_BIT_CS51);     /* /8, timer runs */

    return id;
}

/* echo pulse width in us, ULTRA_NO_ECHO_US on timeout or wrong id */
unsigned int ultra_get_us(unsigned char id)
{
    unsigned int ticks;

    if (id >= ultra_count)
    {
        return ULTRA_NO_ECHO_US;
    }

    /* 10 us trigger pulse */
    gpio_set(ultra_trig_port[id], ultra_trig_pin[id]);
    ultra_wait_ticks(ULTRA_TRIG_TICKS);
    gpio_clear(ultra_trig_port[id], ultra_trig_pin[id]);

    /* wait echo high */
    ultra_timer_clear();
    while (gpio_get(ultra_echo_port[id], ultra_echo_pin[id]) == 0)
    {
        if (ultra_timer_read() >= ULTRA_TIMEOUT_TICKS)
        {
            return ULTRA_NO_ECHO_US;
        }
    }

    /* measure how long echo stays high */
    ultra_timer_clear();
    while (gpio_get(ultra_echo_port[id], ultra_echo_pin[id]) == 1)
    {
        if (ultra_timer_read() >= ULTRA_TIMEOUT_TICKS)
        {
            return ULTRA_NO_ECHO_US;
        }
    }
    ticks = ultra_timer_read();

    return ticks / 2;   /* 2 ticks = 1 us */
}

/* distance in cm, ULTRA_NO_ECHO on timeout or wrong id */
unsigned int ultra_get_cm(unsigned char id)
{
    unsigned int us = ultra_get_us(id);

    if (us == ULTRA_NO_ECHO_US)
    {
        return ULTRA_NO_ECHO;
    }

    /* sound: 58 us per cm, there and back */
    return us / 58;
}

/* average cm of n reads (n 1..8), 60 ms apart, ULTRA_NO_ECHO if all fail */
unsigned int ultra_get_cm_avg(unsigned char id, unsigned char n)
{
    unsigned long sum = 0;
    unsigned char good = 0;
    unsigned char i;
    unsigned int cm;

    if (id >= ultra_count || n < 1 || n > 8)
    {
        return ULTRA_NO_ECHO;   /* wrong id or n */
    }

    for (i = 0; i < n; i++)
    {
        cm = ultra_get_cm(id);
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
