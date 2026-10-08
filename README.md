# ATmega2560 Bare-Metal Drivers

Simple drivers for the ATmega2560 (Arduino Mega 2560 board, 16 MHz) written
in plain C with avr-gcc. No Arduino functions or libraries are used. Only
`<avr/io.h>`, `<avr/interrupt.h>` and `<util/delay.h>`.

Drivers: GPIO, Timer, PWM and ADC.

Author: Ponmudi

## Status

| Module | Files                    | Status      |
|--------|--------------------------|-------------|
| GPIO   | hal/m2560_gpio.c / .h    | done        |
| Timer  | hal/m2560_timer.c / .h   | in progress |
| PWM    | hal/m2560_pwm.c / .h     | in progress |
| ADC    | hal/m2560_adc.c / .h     | in progress |

## Folder layout

```
atmega2560-baremetal-drivers/
├── hal/                      drivers
│   ├── m2560_gpio.c / .h     GPIO driver
│   ├── m2560_timer.c / .h    Timer1 1 ms tick
│   ├── m2560_pwm.c / .h      Timer4 PWM
│   └── m2560_adc.c / .h      ADC driver
├── examples/                 one main() per file
│   ├── ex1_gpio_blink.c
│   ├── ex2_timer_blink.c
│   ├── ex3_pwm_fade.c
│   └── ex4_adc_pot_dimmer.c
├── Makefile
└── README.md
```

Build output goes into `build/` (not in git).

## GPIO driver

Port is a letter `'A'`..`'L'` (there is no port I on the ATmega2560).
Pin is 0..7. A wrong port or pin is ignored.

```c
#define M2560_DIR_IN         0
#define M2560_DIR_OUT        1
#define M2560_DIR_IN_PULLUP  2

void          m2560_gpio_dir(char port, unsigned char pin, unsigned char dir);
void          m2560_gpio_set(char port, unsigned char pin);
void          m2560_gpio_clear(char port, unsigned char pin);
unsigned char m2560_gpio_get(char port, unsigned char pin);    /* 0 or 1 */
void          m2560_gpio_invert(char port, unsigned char pin);
```

| Register | What it does |
|----------|--------------|
| DDRx     | direction of each pin, 1 = output, 0 = input |
| PORTx    | output level (1 = high). On an input pin, 1 turns the pull-up on |
| PINx     | reads the real level on the pin |

## Timer driver

Timer1 in CTC mode gives a 1 ms tick: 16 MHz / 64 = 250 kHz, and
OCR1A = 249 means a compare match every 250 counts = 1 ms.

```c
void          m2560_timer_init(void);
unsigned long m2560_timer_millis(void);
void          m2560_timer_wait_ms(unsigned long ms);
```

| Register | What it does |
|----------|--------------|
| TCCR1A   | Timer1 control A (WGM10/WGM11 stay 0 for CTC) |
| TCCR1B   | WGM12 = CTC mode, CS11 + CS10 = prescaler 64 |
| OCR1A    | compare value 249, timer resets after this |
| TIMSK1   | OCIE1A enables the compare match A interrupt |
| SREG     | global interrupt flag, saved/restored when reading millis |

## PWM driver

Timer4, Fast PWM 8-bit, prescaler 64, non-inverting, about 976 Hz.

| Channel | Pin | Board pin |
|---------|-----|-----------|
| 'A'     | OC4A = PH3 | D6 |
| 'B'     | OC4B = PH4 | D7 |
| 'C'     | OC4C = PH5 | D8 |

```c
void m2560_pwm_init(char channel);
void m2560_pwm_duty(char channel, unsigned char duty);   /* 0..255 */
void m2560_pwm_stop(char channel);
```

| Register | What it does |
|----------|--------------|
| TCCR4A   | COM4x1 connects the OC4x pin, WGM40 for Fast PWM 8-bit |
| TCCR4B   | WGM42 for Fast PWM 8-bit, CS41 + CS40 = prescaler 64 |
| OCR4A/B/C| duty value for each channel |
| DDRH     | OC4x pins must be outputs (set with the GPIO driver) |

## ADC driver

Reference AVcc, right-adjusted result, prescaler 128 (125 kHz ADC clock).
Channels 0..15 (A0..A15).

```c
void         m2560_adc_init(void);
unsigned int m2560_adc_sample(unsigned char channel);      /* 0..1023 */
unsigned int m2560_adc_to_millivolts(unsigned int raw);    /* raw * 5000 / 1023 */
```

| Register | What it does |
|----------|--------------|
| ADMUX    | REFS0 = AVcc reference, MUX4:0 = channel (low part) |
| ADCSRB   | MUX5 = 1 for channels 8..15 |
| ADCSRA   | ADEN turns ADC on, ADSC starts a conversion, ADPS2:0 = prescaler 128 |
| ADC (ADCL/ADCH) | 10-bit result |

## Wiring for the examples

| Example | What it does | Wiring |
|---------|--------------|--------|
| ex1_gpio_blink     | blink onboard LED every 500 ms | nothing, uses the onboard LED (D13 / PB7) |
| ex2_timer_blink    | toggle onboard LED every 1000 ms, non-blocking | nothing, onboard LED |
| ex3_pwm_fade       | fade an LED up and down | D6 -> 220 ohm resistor -> LED (+) , LED (-) -> GND |
| ex4_adc_pot_dimmer | pot sets LED brightness | pot ends to 5V and GND, middle pin to A0. LED on D6 as in ex3 |

## Build and flash

Install the tools on Fedora:

```
sudo dnf install avr-gcc avr-libc avrdude
```

Build, flash, clean:

```
make EX=ex1_gpio_blink
make flash EX=ex1_gpio_blink
make clean
```

The default serial port is `/dev/ttyACM0`. Clone boards with a CH340 chip
usually show up as `/dev/ttyUSB0`:

```
make flash EX=ex1_gpio_blink PORT=/dev/ttyUSB0
```

`avr-size` runs after every build and prints flash and RAM use.

## Author

Ponmudi
