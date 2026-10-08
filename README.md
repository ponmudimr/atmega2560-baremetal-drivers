# ATmega2560 Bare-Metal Drivers

Simple drivers for the ATmega2560 (Arduino Mega 2560 board, 16 MHz) written
in plain C with avr-gcc. No Arduino functions or libraries are used.

No headers from avr-libc. Every register is written by its datasheet
address (see hal/m2560_regs.h).

Drivers: GPIO for now. More will be added after review.

Author: Ponmudi

## Status

| Module    | Files                    | Status                          |
|-----------|--------------------------|---------------------------------|
| Registers | hal/m2560_regs.h         | done                            |
| GPIO      | hal/m2560_gpio.c / .h    | builds, not yet tested on board |

## Folder layout

```
atmega2560-baremetal-drivers/
├── hal/                      drivers
│   ├── m2560_regs.h          register addresses
│   └── m2560_gpio.c / .h     GPIO driver
├── examples/                 one main() per file
│   └── ex1_gpio_blink.c
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

Other datasheet parts used: Register Summary (p.399), I/O-Ports (p.96-100).

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

## Wiring for the examples

| Example | What it does | Wiring |
|---------|--------------|--------|
| ex1_gpio_blink | blink onboard LED about every 500 ms (busy loop) | nothing, uses the onboard LED (D13 / PB7) |

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
