# ATmega2560 Bare-Metal Drivers

Simple drivers for the ATmega2560 (Arduino Mega 2560 board, 16 MHz) written
in plain C with avr-gcc. No Arduino functions or libraries are used.

No headers from avr-libc. Every register is written by its datasheet
address (see hal/m2560_regs.h).

Drivers: GPIO, Timer, PWM and ADC.

Author: Ponmudi

## Status

| Module    | Files                    | Status                          |
|-----------|--------------------------|---------------------------------|
| Registers | hal/m2560_regs.h         | done                            |
| GPIO      | hal/m2560_gpio.c / .h    | builds, not yet tested on board |
| Timer     | hal/m2560_timer.c / .h   | builds, not yet tested on board |
| PWM       | hal/m2560_pwm.c / .h     | builds, not yet tested on board |
| ADC       | hal/m2560_adc.c / .h     | builds, not yet tested on board |

## Folder layout

```
atmega2560-baremetal-drivers/
├── hal/                      drivers
│   ├── m2560_regs.h          register addresses and bit numbers
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

## Register addresses

The datasheet Register Summary shows two addresses for low registers,
for example `0x04 (0x24) DDRB`. The first one is the I/O address, only for
the IN/OUT instructions. The one in brackets is the data space (memory)
address. A C pointer must use the data space address, so this project
uses the number in brackets.

Datasheet: Atmel ATmega640/1280/1281/2560/2561, doc 2549Q-AVR-02/2014.

| Register | Address | Datasheet page |
|----------|---------|----------------|
| PINA / DDRA / PORTA | 0x20 / 0x21 / 0x22 | 96 |
| PINB / DDRB / PORTB | 0x23 / 0x24 / 0x25 | 96 |
| PINC / DDRC / PORTC | 0x26 / 0x27 / 0x28 | 97 |
| PIND / DDRD / PORTD | 0x29 / 0x2A / 0x2B | 97 |
| PINE / DDRE / PORTE | 0x2C / 0x2D / 0x2E | 97-98 |
| PINF / DDRF / PORTF | 0x2F / 0x30 / 0x31 | 97-98 |
| PING / DDRG / PORTG | 0x32 / 0x33 / 0x34 | 98 |
| PINH / DDRH / PORTH | 0x100 / 0x101 / 0x102 | 98-99 |
| PINJ / DDRJ / PORTJ | 0x103 / 0x104 / 0x105 | 99 |
| PINK / DDRK / PORTK | 0x106 / 0x107 / 0x108 | 99 |
| PINL / DDRL / PORTL | 0x109 / 0x10A / 0x10B | 100 |
| SREG     | 0x5F (I/O 0x3F) | 13 |
| TIFR1    | 0x36 (I/O 0x16) | 162 |
| TIMSK1   | 0x6F | 161 |
| TCCR1A   | 0x80 | 154 |
| TCCR1B   | 0x81 | 156 |
| TCNT1L / TCNT1H | 0x84 / 0x85 | 158 |
| OCR1AL / OCR1AH | 0x88 / 0x89 | 159 |
| TCCR4A   | 0xA0 | 154 |
| TCCR4B   | 0xA1 | 156 |
| OCR4AL / OCR4AH | 0xA8 / 0xA9 | 159 |
| OCR4BL / OCR4BH | 0xAA / 0xAB | 160 |
| OCR4CL / OCR4CH | 0xAC / 0xAD | 160 |
| ADCL / ADCH | 0x78 / 0x79 | 286 |
| ADCSRA   | 0x7A | 285 |
| ADCSRB   | 0x7B | 287 |
| ADMUX    | 0x7C | 281 |
| DIDR2    | 0x7D | 288 |
| DIDR0    | 0x7E | 287 |

Other datasheet parts used: Register Summary (p.399), Reset and Interrupt
Vectors Table 14-1 (p.101), Accessing 16-bit Registers (p.135), ADC Input
Channel Selections Table 26-4.

Interrupts: global on/off is done by setting/clearing bit I (bit 7) of SREG.
The Timer1 compare A handler is a function named `__vector_17`, because the
datasheet calls it vector No. 18 counting from 1, and avr-gcc counts from 0.

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
| TCNT1H/L | the counter itself, set to 0 at start |
| OCR1AH/L | compare value 249, timer resets after this (high byte written first) |
| TIMSK1   | OCIE1A enables the compare match A interrupt |
| TIFR1    | OCF1A flag, cleared at start by writing 1 |
| SREG     | bit I = global interrupts on/off, saved/restored when reading millis |

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
| ADCL, ADCH | 10-bit result, read ADCL first, then ADCH |
| DIDR0, DIDR2 | digital input disable, left alone (explained in m2560_adc.c) |

## Wiring for the examples

| Example | What it does | Wiring |
|---------|--------------|--------|
| ex1_gpio_blink     | blink onboard LED about every 500 ms (busy loop) | nothing, uses the onboard LED (D13 / PB7) |
| ex2_timer_blink    | toggle onboard LED every 1000 ms, non-blocking | nothing, onboard LED |
| ex3_pwm_fade       | fade an LED up and down | D6 -> 220 ohm resistor -> LED (+) , LED (-) -> GND |
| ex4_adc_pot_dimmer | pot sets LED brightness | pot ends to 5V and GND, middle pin to A0. LED on D6 as in ex3 |

## Build and flash

`-mmcu=atmega2560` is still needed so the compiler knows the CPU. avr-gcc
still links its startup code (stack setup, vector table), which is part of
the compiler toolchain, not a peripheral library.

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
