/*
 * ultra.c - ultrasonic sensor (HC-SR04 type), uses Timer5
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

/* set pins, start Timer5 */
void ultra_init(void)
{
    gpio_dir(ULTRA_PORT, ULTRA_TRIG_PIN, GPIO_OUT);
    gpio_clear(ULTRA_PORT, ULTRA_TRIG_PIN);         /* trig low */
    gpio_dir(ULTRA_PORT, ULTRA_ECHO_PIN, GPIO_IN);

    M2560_TCCR5A = 0;                         /* normal mode */
    M2560_TCCR5B = (1 << M2560_BIT_CS51);     /* /8, timer runs */
}

/* measure distance in cm, 999 if no echo */
unsigned int ultra_get_cm(void)
{
    unsigned int ticks;

    /* 10 us trigger pulse */
    ultra_timer_clear();
    gpio_set(ULTRA_PORT, ULTRA_TRIG_PIN);
    while (ultra_timer_read() < ULTRA_TRIG_TICKS)
    {
    }
    gpio_clear(ULTRA_PORT, ULTRA_TRIG_PIN);

    /* wait echo high */
    ultra_timer_clear();
    while (gpio_get(ULTRA_PORT, ULTRA_ECHO_PIN) == 0)
    {
        if (ultra_timer_read() >= ULTRA_TIMEOUT_TICKS)
        {
            return ULTRA_NO_ECHO;
        }
    }

    /* measure how long echo stays high */
    ultra_timer_clear();
    while (gpio_get(ULTRA_PORT, ULTRA_ECHO_PIN) == 1)
    {
        if (ultra_timer_read() >= ULTRA_TIMEOUT_TICKS)
        {
            return ULTRA_NO_ECHO;
        }
    }
    ticks = ultra_timer_read();

    /* cm = us / 58 = ticks / 116 */
    return ticks / 116;
}
