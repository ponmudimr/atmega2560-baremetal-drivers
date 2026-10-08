# Makefile - build one test program for ATmega2560
# Author: Ponmudi
#
# make TEST=t_seg7          build tests/t_seg7.c
# make flash TEST=t_seg7    build and upload
# make app                  build app/main.c
# make flash-app            build app/main.c and upload
# make clean                delete build/
#
# CH340 clone boards: make flash TEST=t_seg7 PORT=/dev/ttyUSB0
# Tools on Fedora: sudo dnf install avr-gcc avr-libc avrdude

MCU     = atmega2560
F_CPU   = 16000000UL
TEST   ?= t_led_sw
PORT   ?= /dev/ttyACM0
BAUD    = 115200

CC      = avr-gcc
OBJCOPY = avr-objcopy
SIZE    = avr-size

# -mmcu tells the compiler which CPU it is.
# No avr-libc headers are used. avr-gcc still links its startup
# code (stack setup, vector table). That is part of the compiler
# toolchain, not a peripheral library.

# each driver has its own folder: drivers/gpio, drivers/adc, ...
# regs.h and board.h stay in drivers/ (used by all drivers)
DRV_DIRS = $(sort $(dir $(wildcard drivers/*/*.c)))
DRV_SRC  = $(wildcard drivers/*/*.c)
DRV_INC  = -Idrivers $(addprefix -I,$(DRV_DIRS))

CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra $(DRV_INC)

all: build/$(TEST).hex

build:
	mkdir -p build

build/$(TEST).elf: tests/$(TEST).c $(DRV_SRC) $(wildcard drivers/*.h drivers/*/*.h) | build
	$(CC) $(CFLAGS) -o $@ tests/$(TEST).c $(DRV_SRC)

build/$(TEST).hex: build/$(TEST).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) --format=avr --mcu=$(MCU) $<

flash: build/$(TEST).hex
	avrdude -p m2560 -c wiring -P $(PORT) -b $(BAUD) -D -U flash:w:build/$(TEST).hex

# the application in app/main.c
build/app.elf: app/main.c $(DRV_SRC) $(wildcard drivers/*.h drivers/*/*.h) | build
	$(CC) $(CFLAGS) -o $@ app/main.c $(DRV_SRC)

build/app.hex: build/app.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) --format=avr --mcu=$(MCU) $<

app: build/app.hex

flash-app: build/app.hex
	avrdude -p m2560 -c wiring -P $(PORT) -b $(BAUD) -D -U flash:w:build/app.hex

clean:
	rm -rf build

.PHONY: all flash app flash-app clean
