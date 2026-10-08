/*
 * File    : m2560_pwm.c
 * About   : PWM on Timer4, Fast PWM 8-bit mode (WGM43..40 = 0101).
 *           Timer counts 0..255 again and again. Non-inverting mode:
 *           pin goes high at 0 and low when count matches OCR4x,
 *           so a bigger OCR4x = longer high time = brighter LED.
 *           Frequency = 16 MHz / 64 / 256 = ~976 Hz.
 * Registers used : TCCR4A, TCCR4B, OCR4AH/L, OCR4BH/L, OCR4CH/L
 *           (DDRH/PORTH through m2560_gpio)
 *           see datasheet: 16-bit Timer/Counter, Fast PWM Mode
 * Author  : Ponmudi
 */

#include "m2560_regs.h"
#include "m2560_gpio.h"
#include "m2560_pwm.h"

/*
 * m2560_pwm_init - set up Timer4 and start PWM on one channel
 * channel : 'A', 'B' or 'C' (other letters are ignored)
 * returns nothing
 */
void m2560_pwm_init(char channel)
{
    /* first check channel, make its pin an output, connect OC4x pin */
    switch (channel)
    {
        case 'A':
            m2560_gpio_dir('H', 3, M2560_DIR_OUT);       /* PH3 = D6 output */
            M2560_TCCR4A |= (1 << M2560_BIT_COM4A1);     /* OC4A non-inverting */
            break;
        case 'B':
            m2560_gpio_dir('H', 4, M2560_DIR_OUT);       /* PH4 = D7 output */
            M2560_TCCR4A |= (1 << M2560_BIT_COM4B1);     /* OC4B non-inverting */
            break;
        case 'C':
            m2560_gpio_dir('H', 5, M2560_DIR_OUT);       /* PH5 = D8 output */
            M2560_TCCR4A |= (1 << M2560_BIT_COM4C1);     /* OC4C non-inverting */
            break;
        default:
            return;   /* wrong channel, do nothing */
    }

    /* Fast PWM 8-bit: WGM40 = 1 and WGM42 = 1 (WGM41, WGM43 stay 0 from reset) */
    M2560_TCCR4A |= (1 << M2560_BIT_WGM40);
    M2560_TCCR4B |= (1 << M2560_BIT_WGM42);

    /* prescaler 64: CS41 + CS40, timer starts running now */
    M2560_TCCR4B |= (1 << M2560_BIT_CS41);
    M2560_TCCR4B |= (1 << M2560_BIT_CS40);
}

/*
 * m2560_pwm_duty - change the duty cycle of a channel
 * channel : 'A', 'B' or 'C'
 * duty    : 0..255 (0 = almost off, 255 = fully on)
 * returns nothing
 * note: in Fast PWM, duty 0 still gives a very short pulse each cycle
 */
void m2560_pwm_duty(char channel, unsigned char duty)
{
    /* OCR4x is 16-bit: write high byte first, then low byte.
       The high byte goes to a TEMP register and both bytes are
       stored together when the low byte is written. */
    switch (channel)
    {
        case 'A':
            M2560_OCR4AH = 0;
            M2560_OCR4AL = duty;
            break;
        case 'B':
            M2560_OCR4BH = 0;
            M2560_OCR4BL = duty;
            break;
        case 'C':
            M2560_OCR4CH = 0;
            M2560_OCR4CL = duty;
            break;
        default:
            break;    /* wrong channel, ignore */
    }
}

/*
 * m2560_pwm_stop - stop PWM on a channel and make the pin low
 * channel : 'A', 'B' or 'C'
 * returns nothing
 * the timer keeps running so other channels are not affected
 */
void m2560_pwm_stop(char channel)
{
    switch (channel)
    {
        case 'A':
            M2560_TCCR4A &= ~(1 << M2560_BIT_COM4A1);    /* disconnect OC4A, pin is normal GPIO again */
            m2560_gpio_clear('H', 3);                    /* drive D6 low */
            break;
        case 'B':
            M2560_TCCR4A &= ~(1 << M2560_BIT_COM4B1);    /* disconnect OC4B */
            m2560_gpio_clear('H', 4);                    /* drive D7 low */
            break;
        case 'C':
            M2560_TCCR4A &= ~(1 << M2560_BIT_COM4C1);    /* disconnect OC4C */
            m2560_gpio_clear('H', 5);                    /* drive D8 low */
            break;
        default:
            break;    /* wrong channel, ignore */
    }
}
