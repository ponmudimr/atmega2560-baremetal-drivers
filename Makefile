# Makefile for ATmega2560 bare-metal drivers
# Author : Ponmudi
#
# make EX=ex1_gpio_blink          -> build one example
# make flash EX=ex1_gpio_blink    -> build and upload to the board
# make clean                      -> delete build/
#
# Clone boards with CH340 chip use /dev/ttyUSB0:
# make flash EX=ex1_gpio_blink PORT=/dev/ttyUSB0
#
# Toolchain missing? On Fedora:
# sudo dnf install avr-gcc avr-libc avrdude

MCU     = atmega2560
F_CPU   = 16000000UL
EX     ?= ex1_gpio_blink
PORT   ?= /dev/ttyACM0
BAUD    = 115200

CC      = avr-gcc
OBJCOPY = avr-objcopy
SIZE    = avr-size

# -mmcu=atmega2560 is still needed so the compiler knows the CPU
# (instruction set, flash/RAM size, where the vector table goes).
# Our code includes no avr-libc headers. avr-gcc still links its
# startup code (sets up the stack, clears RAM, vector table that
# jumps to __vector_N and then calls main). That is part of the
# compiler toolchain, not a peripheral library.
CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -Ihal

# all driver .c files in hal/
HAL_SRC = $(wildcard hal/*.c)

all: build/$(EX).hex

build:
	mkdir -p build

build/$(EX).elf: examples/$(EX).c $(HAL_SRC) $(wildcard hal/*.h) | build
	$(CC) $(CFLAGS) -o $@ examples/$(EX).c $(HAL_SRC)

build/$(EX).hex: build/$(EX).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) --format=avr --mcu=$(MCU) $<

flash: build/$(EX).hex
	avrdude -p m2560 -c wiring -P $(PORT) -b $(BAUD) -D -U flash:w:build/$(EX).hex

clean:
	rm -rf build

.PHONY: all flash clean
