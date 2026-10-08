# ATmega2560 Smart Parking - Day 1 Drivers

Bare-metal drivers for the ATmega2560 (Arduino Mega 2560, 16 MHz) in plain C
with avr-gcc. No Arduino code and no libraries.

No headers from avr-libc. Every register is written by its datasheet
address (see drivers/regs.h). Test programs use driver functions only.

Author: Ponmudi

## Folder layout

```
app/main.c        parking app (Day 2, empty now)
drivers/
  regs.h          register addresses (data space) and bit numbers
  board.h         pin map
  gpio.c/.h       pin direction, set, clear, read
  led.c/.h        5 status LEDs
  sw.c/.h         push switch
  timer.c/.h      1 ms tick, millis, delay
  seg7.c/.h       2-digit 7-segment display
  ir.c/.h         IR slot sensor
  ultra.c/.h      ultrasonic distance
  pwm.c/.h        buzzer PWM
  adc.c/.h        analog read, channels 0..7
tests/            one test program per driver + gate test
Makefile
```

## Pin map

| Signal | Port pin | Board pin |
|--------|----------|-----------|
| LED SAFE | PA0 | D22 |
| LED CAUTION | PA1 | D23 |
| LED WARNING | PA2 | D24 |
| LED STOP | PA3 | D25 |
| LED OCCUPIED | PA4 | D26 |
| Switch (to GND, pull-up) | PE4 | D2 |
| 7-seg a..g, dp | PC0..PC7 | D37..D30 |
| 7-seg digit 1 (tens) | PG0 | D41 |
| 7-seg digit 2 (ones) | PG1 | D40 |
| Ultrasonic TRIG | PL2 | D47 |
| Ultrasonic ECHO | PL1 | D48 |
| IR sensor | PL3 | D46 |
| Buzzer (OC4A) | PH3 | D6 |
| Pot | ADC0 | A0 |

## Hardware assumptions (hardcoded in the drivers)

- 7-segment is common cathode: segment pin HIGH = segment on.
- Digit pin HIGH = digit on (digit driven through a transistor).
- IR sensor output is LOW when a car is in the slot.
- ADC uses channels 0..7 only (pot on channel 0).

If the board is wired differently, the driver code must change.

## Which timer does what

| Timer | Used by | Setup | Result |
|-------|---------|-------|--------|
| Timer0 | seg7 | CTC, /256, OCR0A = 124 | interrupt every 2 ms, shows next digit |
| Timer1 | timer | CTC, /64, OCR1A = 249 | interrupt every 1 ms, millis count |
| Timer4 | pwm | mode 14 fast PWM, /8, ICR4 = 999 | 2 kHz on OC4A |
| Timer5 | ultra | normal, /8 | 0.5 us per tick for echo timing |

Interrupt handlers (no avr-libc): `__vector_17` = Timer1 COMPA (datasheet
vector No.18), `__vector_21` = Timer0 COMPA (No.22). avr-gcc counts vectors
from 0, the datasheet from 1.

16-bit registers: write the high byte first, read the low byte first.

## Register addresses

The datasheet shows two addresses for low registers, like `0x04 (0x24) DDRB`.
0x04 is for IN/OUT, 0x24 (in brackets) is the memory address. C pointers use
the memory one. Datasheet: Atmel 2549Q-AVR-02/2014.

| Register | Address | Page |
|----------|---------|------|
| PINA/DDRA/PORTA | 0x20/0x21/0x22 | 96 |
| PINC/DDRC/PORTC | 0x26/0x27/0x28 | 97 |
| PINE/DDRE/PORTE | 0x2C/0x2D/0x2E | 97-98 |
| PING/DDRG/PORTG | 0x32/0x33/0x34 | 98 |
| PINH/DDRH/PORTH | 0x100/0x101/0x102 | 98-99 |
| PINL/DDRL/PORTL | 0x109/0x10A/0x10B | 100 |
| SREG | 0x5F | 13 |
| TCCR0A / TCCR0B / OCR0A / TIMSK0 | 0x44 / 0x45 / 0x47 / 0x6E | 126-131 |
| TCCR1A / TCCR1B / OCR1AL-H / TIMSK1 | 0x80 / 0x81 / 0x88-0x89 / 0x6F | 154-161 |
| TCCR4A / TCCR4B / ICR4L-H / OCR4AL-H | 0xA0 / 0xA1 / 0xA6-0xA7 / 0xA8-0xA9 | 154-161 |
| TCCR5A / TCCR5B / TCNT5L-H | 0x120 / 0x121 / 0x124-0x125 | 154-158 |
| ADCL / ADCH / ADCSRA / ADMUX | 0x78 / 0x79 / 0x7A / 0x7C | 281-286 |

regs.h only has the ports and registers this project uses.

## Tests

| Test | What it shows |
|------|---------------|
| t_led_sw | LEDs on one by one, then STOP LED on while switch is pressed |
| t_timer | SAFE LED toggles every 1000 ms (timer_millis) |
| t_seg7 | counts 0..99 every 300 ms (one digit, then two digits) |
| t_ir | OCCUPIED LED on when slot is occupied |
| t_ultra | distance in cm on 7-seg every 100 ms, dash if > 99 or no echo |
| t_pwm | buzzer at 25%, 50%, 75% duty, 1 s each |
| t_adc | pot value / 11 on 7-seg |
| t_gate | occupied -> dash + OCCUPIED LED, else distance on 7-seg |

## Build and flash

Install tools on Fedora:

```
sudo dnf install avr-gcc avr-libc avrdude
```

Build, flash, clean (any test name from the table):

```
make TEST=t_led_sw
make TEST=t_timer
make TEST=t_seg7
make TEST=t_ir
make TEST=t_ultra
make TEST=t_pwm
make TEST=t_adc
make TEST=t_gate

make flash TEST=t_seg7
make clean
```

Default port is /dev/ttyACM0. CH340 clone boards: `make flash TEST=t_seg7 PORT=/dev/ttyUSB0`.

`-mmcu=atmega2560` is still needed so the compiler knows the CPU. avr-gcc
links its startup code (stack setup, vector table), which is part of the
toolchain, not a peripheral library. avr-size runs after each build.

## Author

Ponmudi
