/*
 * pwm.c - PWM on Timer4 (mode 14, /8, top = ICR4)
 *         'A' = OC4A = PH3 (D6), 'B' = OC4B = PH4 (D7), 'C' = OC4C = PH5 (D8)
 * Author: Ponmudi
 */

#include "regs.h"
#include "gpio.h"
#include "pwm.h"

#define PWM_PORT  'H'   /* OC4A/B/C are fixed on port H */

/* 1 after Timer4 is set up */
static unsigned char pwm_started = 0;

/* ICR4 value, 999 = 2 kHz */
static unsigned int pwm_top = 999;

/* duty of each channel, kept for pwm_set_freq */
static unsigned char pwm_duty[3] = {0, 0, 0};

/* channel letter to 0..2, 255 if wrong */
static unsigned char pwm_index(char ch)
{
    switch (ch)
    {
        case 'A': return 0;
        case 'B': return 1;
        case 'C': return 2;
        default:  return 255;   /* wrong channel */
    }
}

/* pin on port H for a channel index */
static unsigned char pwm_pin(unsigned char idx)
{
    switch (idx)
    {
        case 0:  return 3;   /* PH3 */
        case 1:  return 4;   /* PH4 */
        default: return 5;   /* PH5 */
    }
}

/* COM bit in TCCR4A for a channel index */
static unsigned char pwm_com_bit(unsigned char idx)
{
    switch (idx)
    {
        case 0:  return M2560_BIT_COM4A1;
        case 1:  return M2560_BIT_COM4B1;
        default: return M2560_BIT_COM4C1;
    }
}

/* write OCR4A/B/C, high byte first */
static void pwm_write_ocr(unsigned char idx, unsigned int value)
{
    unsigned char high_byte = (unsigned char)(value >> 8);
    unsigned char low_byte = (unsigned char)(value & 0xFF);

    switch (idx)
    {
        case 0:
            M2560_OCR4AH = high_byte;
            M2560_OCR4AL = low_byte;
            break;
        case 1:
            M2560_OCR4BH = high_byte;
            M2560_OCR4BL = low_byte;
            break;
        default:
            M2560_OCR4CH = high_byte;
            M2560_OCR4CL = low_byte;
            break;
    }
}

/* duty 0..100 to OCR value: (top + 1) * duty / 100 */
static unsigned int pwm_duty_to_ocr(unsigned char duty)
{
    unsigned long value = ((unsigned long)pwm_top + 1) * duty / 100;

    if (value > 65535)
    {
        value = 65535;   /* 100% with top 65535 */
    }

    return (unsigned int)value;
}

/* write ICR4 (top), high byte first */
static void pwm_write_top(void)
{
    M2560_ICR4H = (unsigned char)(pwm_top >> 8);
    M2560_ICR4L = (unsigned char)(pwm_top & 0xFF);
}

/* set channel pin as output, start Timer4 once (2 kHz), output off */
void pwm_init(char ch)
{
    unsigned char idx = pwm_index(ch);

    if (idx == 255)
    {
        return;
    }

    gpio_dir(PWM_PORT, pwm_pin(idx), GPIO_OUT);
    gpio_clear(PWM_PORT, pwm_pin(idx));

    if (pwm_started == 0)
    {
        /* mode 14: fast PWM, top = ICR4, no pins connected yet */
        M2560_TCCR4A = (1 << M2560_BIT_WGM41);
        M2560_TCCR4B = (1 << M2560_BIT_WGM43) | (1 << M2560_BIT_WGM42);
        pwm_write_top();
        M2560_TCCR4B |= (1 << M2560_BIT_CS41);   /* /8, timer starts */
        pwm_started = 1;
    }

    pwm_set_duty(ch, 0);
}

/* frequency 31..65535 Hz, same for all 3 channels */
void pwm_set_freq(unsigned int hz)
{
    unsigned char idx;

    if (hz < 31)
    {
        return;   /* top would not fit in 16 bits */
    }

    /* 16 MHz / 8 = 2 MHz, f = 2 MHz / (top + 1) */
    pwm_top = (unsigned int)(2000000UL / hz - 1);

    if (pwm_started)
    {
        pwm_write_top();
    }

    /* top changed, so put each duty back */
    for (idx = 0; idx < 3; idx++)
    {
        pwm_write_ocr(idx, pwm_duty_to_ocr(pwm_duty[idx]));
    }
}

/* duty 0..100 percent for one channel */
void pwm_set_duty(char ch, unsigned char duty)
{
    unsigned char idx = pwm_index(ch);

    if (idx == 255 || duty > 100)
    {
        return;
    }

    pwm_duty[idx] = duty;
    pwm_write_ocr(idx, pwm_duty_to_ocr(duty));
}

/* connect PWM to the channel pin */
void pwm_on(char ch)
{
    unsigned char idx = pwm_index(ch);

    if (idx == 255)
    {
        return;
    }

    M2560_TCCR4A |= (1 << pwm_com_bit(idx));    /* non-inverting */
}

/* disconnect PWM, pin low */
void pwm_off(char ch)
{
    unsigned char idx = pwm_index(ch);

    if (idx == 255)
    {
        return;
    }

    M2560_TCCR4A &= ~(1 << pwm_com_bit(idx));   /* pin back to normal GPIO */
    gpio_clear(PWM_PORT, pwm_pin(idx));
}
