/*
 * pwm.c - 2 kHz PWM for buzzer on OC4A (Timer4, mode 14)
 * Author: Ponmudi
 */

#include "regs.h"
#include "board.h"
#include "gpio.h"
#include "pwm.h"

/* set pin, start Timer4, output stays off */
void pwm_init(void)
{
    gpio_dir(BUZZER_PORT, BUZZER_PIN, GPIO_OUT);
    gpio_clear(BUZZER_PORT, BUZZER_PIN);

    /* mode 14: fast PWM, top = ICR4 */
    M2560_TCCR4A = (1 << M2560_BIT_WGM41);     /* pin not connected yet */
    M2560_TCCR4B = (1 << M2560_BIT_WGM43) | (1 << M2560_BIT_WGM42);

    /* 16 MHz / 8 / 1000 = 2 kHz, top = 999 = 0x03E7 */
    M2560_ICR4H = 0x03;                        /* high byte first */
    M2560_ICR4L = 0xE7;

    pwm_set_duty(0);

    M2560_TCCR4B |= (1 << M2560_BIT_CS41);     /* /8, timer starts */
}

/* duty in percent, 0..100 */
void pwm_set_duty(unsigned char duty)
{
    unsigned int value;

    if (duty > 100)
    {
        duty = 100;
    }

    value = duty * 10;   /* 0..1000, top is 999 */

    M2560_OCR4AH = (unsigned char)(value >> 8);   /* high byte first */
    M2560_OCR4AL = (unsigned char)(value & 0xFF);
}

/* connect PWM to pin */
void pwm_on(void)
{
    M2560_TCCR4A |= (1 << M2560_BIT_COM4A1);   /* OC4A non-inverting */
}

/* disconnect PWM, pin low */
void pwm_off(void)
{
    M2560_TCCR4A &= ~(1 << M2560_BIT_COM4A1);  /* pin back to normal GPIO */
    gpio_clear(BUZZER_PORT, BUZZER_PIN);
}
