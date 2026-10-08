# Smart Parking - Day 1 Drivers (Arduino Mega 2560)

This project is the first part of a **smart parking helper**. It measures how
far a car is from the wall, checks if a parking slot is free, and shows this
with LEDs, a number display and a buzzer.

Day 1 is only about the **drivers**: small pieces of C code that talk to the
hardware. Day 2 will use them to build the full parking app.

Author: Ponmudi

---

## 1. Start here: what is the board?

You have an **Arduino Mega 2560**. It is a *development board*: a ready-made
circuit board that makes it easy to try out a chip.

- The big black chip in the middle is the **ATmega2560**. This is the real
  "computer". It is a **microcontroller**: a tiny computer with its own
  memory, and pins that can switch things on and off or read sensors.
- The chip runs at **16 MHz** (16 million steps per second).
- The **USB cable** gives the board power and is used to send (upload) our
  program to the chip.
- The black headers around the edge are the **pins**. They are printed on the
  board as `D0`..`D53` (digital) and `A0`..`A15` (analog).
- There is a small LED on the board marked **L**. It is wired to pin D13.

### Board pin names vs chip pin names

The board prints names like **D22**. Inside the chip, the same wire is called
**PA0**, which means **port A, pin 0**.

The chip groups its pins in **ports** of 8 pins (port A, B, C ... L). Our code
uses the chip names, because that is how the chip works. The wiring tables
below show both names, so you only need to look at the board name (`D22`)
when plugging wires.

---

## 2. Words you will see

| Word | Simple meaning |
|------|----------------|
| Microcontroller | A small computer on one chip (here: ATmega2560) |
| Pin | One metal leg of the chip, can be ON (5V) or OFF (0V), or read a voltage |
| Port | A group of 8 pins (PA0..PA7 is port A) |
| Register | A special memory box inside the chip. Writing a number into it controls the hardware (for example "make this pin an output") |
| Datasheet | The chip's manual from the maker. It lists every register and its address |
| Driver | Our C code that writes the registers for you, so the rest of the program just calls `led_on(...)` |
| Bare-metal | No operating system and no ready-made libraries. Our code talks to the registers directly |
| Timer | A counter inside the chip that counts clock ticks. Used to measure time |
| Interrupt | The chip pauses the main program for a moment to run a small function (for example every 1 ms) |
| PWM | Switching a pin on/off very fast. Used here to make the buzzer sound |
| ADC | Analog-to-Digital Converter. Turns a voltage (0..5V) into a number (0..1023) |
| 7-segment display | The number display made of 7 small LED bars (plus a dot) |
| Ultrasonic sensor | Sends a sound pulse and times the echo to measure distance |
| IR sensor | Infrared sensor that tells if something is in front of it |

---

## 3. How the code is organized

The code is built in layers. Each layer only talks to the one below it:

```
 tests/ and app/        "turn on the STOP LED", "what is the distance?"
        |
 drivers/  (led, seg7, ultra, ...)   knows which pin and which register
        |
 drivers/regs.h          the register addresses from the datasheet
        |
 the ATmega2560 chip and the parts wired to the board
```

Rule: the test programs and the app **never** touch registers. They only
call driver functions. This is what the hackathon asks for.

### Folders

```
app/main.c        the parking app (Day 2, empty for now)
drivers/          all the drivers
  regs.h          register addresses (from the datasheet)
  board.h         which part is on which pin (the pin map)
  gpio.c/.h       basic pin control: make pin input/output, set, clear, read
  led.c/.h        the 5 status LEDs
  sw.c/.h         the push button
  timer.c/.h      time keeping: milliseconds and delays
  seg7.c/.h       the 2-digit number display
  ir.c/.h         IR sensor: is the slot occupied?
  ultra.c/.h      ultrasonic sensor: distance in cm
  pwm.c/.h        buzzer sound
  adc.c/.h        read a voltage (the knob / potentiometer)
tests/            one small test program for each driver
Makefile          the build instructions (used by the `make` command)
```

### Suggested reading order

1. `drivers/board.h`: see where every part is connected.
2. `drivers/gpio.c`: everything else is built on this.
3. `drivers/led.c` and `drivers/sw.c`: the simplest drivers.
4. `drivers/timer.c`: the first one with an interrupt.
5. `drivers/seg7.c`, `ir.c`, `ultra.c`, `pwm.c`, `adc.c`.
6. Each test in `tests/`: short programs showing how to use a driver.

### Driver functions (the API)

| Driver | Functions |
|--------|-----------|
| gpio | `gpio_dir(port, pin, dir)`, `gpio_set(port, pin)`, `gpio_clear(port, pin)`, `gpio_get(port, pin)` |
| led | `led_init()`, `led_on(id)`, `led_off(id)`, with ids `LED_SAFE`, `LED_CAUTION`, `LED_WARNING`, `LED_STOP`, `LED_OCCUPIED` |
| sw | `sw_init()`, `sw_is_pressed()` (1 = pressed) |
| timer | `timer_init()`, `timer_millis()`, `timer_delay_ms(ms)` |
| seg7 | `seg7_init()`, `seg7_show_number(0..99)`, `seg7_show_dash()` |
| ir | `ir_init()`, `ir_is_occupied()` (1 = car there) |
| ultra | `ultra_init()`, `ultra_get_cm()` (999 = no echo) |
| pwm | `pwm_init()`, `pwm_set_duty(0..100)`, `pwm_on()`, `pwm_off()` |
| adc | `adc_init()`, `adc_read(0..7)` (returns 0..1023) |

---

## 4. Parts and wiring

You need: the Mega board, a USB cable, a breadboard, jumper wires, 5 LEDs,
resistors (220 ohm for LEDs and segments, 1k for transistors), a push
button, a 2-digit common-cathode 7-segment display, 2 NPN transistors
(for example BC547), an HC-SR04 ultrasonic sensor, an IR obstacle sensor
module, a passive buzzer and a 10k potentiometer.

**GND** means the board's GND pin and **5V** means the board's 5V pin. All
parts must share the same GND.

| Part | Connect to board pin | Chip pin | Notes |
|------|----------------------|----------|-------|
| LED SAFE | D22 | PA0 | pin -> 220 ohm -> LED long leg, short leg -> GND |
| LED CAUTION | D23 | PA1 | same as above |
| LED WARNING | D24 | PA2 | same as above |
| LED STOP | D25 | PA3 | same as above |
| LED OCCUPIED | D26 | PA4 | same as above |
| Push button | D2 | PE4 | one leg to D2, other leg to GND (no resistor needed) |
| 7-seg segment a | D37 | PC0 | each segment through a 220 ohm resistor |
| 7-seg segment b | D36 | PC1 | |
| 7-seg segment c | D35 | PC2 | |
| 7-seg segment d | D34 | PC3 | |
| 7-seg segment e | D33 | PC4 | |
| 7-seg segment f | D32 | PC5 | |
| 7-seg segment g | D31 | PC6 | |
| 7-seg dot (dp) | D30 | PC7 | |
| 7-seg digit 1 (tens, left) | D41 | PG0 | D41 -> 1k -> transistor base, emitter -> GND, collector -> digit 1 common pin |
| 7-seg digit 2 (ones, right) | D40 | PG1 | same, with the second transistor |
| Ultrasonic TRIG | D47 | PL2 | sensor VCC -> 5V, GND -> GND |
| Ultrasonic ECHO | D48 | PL1 | |
| IR sensor OUT | D46 | PL3 | sensor VCC -> 5V, GND -> GND |
| Buzzer + | D6 | PH3 | buzzer - -> GND |
| Potentiometer middle leg | A0 | ADC0 | outer legs -> 5V and GND |

### Things the drivers expect

These are fixed in the code. If your parts work the other way, the driver
code must be changed:

- 7-segment display is **common cathode** (a segment lights when its pin is HIGH).
- A digit turns on when its pin is **HIGH** (through the transistor).
- The IR sensor output is **LOW when a car is in the slot** (most IR modules work like this).
- The ADC reads channels 0..7 only (A0..A7). The pot is on A0.

---

## 5. Install the tools (Fedora Linux)

Open a terminal and run:

```
sudo dnf install avr-gcc avr-libc avrdude make
```

- `avr-gcc` turns our C code into a program the chip understands.
- `avrdude` sends that program to the board over USB.
- `make` runs the build steps written in the `Makefile`.

To let your user talk to the board without `sudo`, add yourself to the
`dialout` group once, then log out and log in again:

```
sudo usermod -aG dialout $USER
```

---

## 6. Build and upload a test, step by step

1. Plug the board into the PC with the USB cable.
2. Find the board's port:
   ```
   ls /dev/ttyACM* /dev/ttyUSB*
   ```
   An original Mega usually shows `/dev/ttyACM0`. Cheap clone boards (with a
   CH340 chip) usually show `/dev/ttyUSB0`.
3. Go into the project folder and build a test (example: the 7-segment test):
   ```
   make TEST=t_seg7
   ```
   If it worked, you see a small table with "Program: ... bytes". That is
   how much of the chip's memory the program uses.
4. Upload it to the board:
   ```
   make flash TEST=t_seg7
   ```
   For a clone board add the port:
   ```
   make flash TEST=t_seg7 PORT=/dev/ttyUSB0
   ```
5. The program starts by itself after upload. Every time the board gets
   power, it runs the last uploaded program.

To delete the build files: `make clean`.

### The tests and what you should see

Build each one with `make TEST=<name>` and upload with `make flash TEST=<name>`.

| Test name | What you should see |
|-----------|---------------------|
| `t_led_sw` | The 5 LEDs turn on one by one, then all go off. After that the STOP LED is on only while you hold the button |
| `t_timer` | The SAFE LED turns on and off every 1 second |
| `t_seg7` | The display counts 0, 1, 2 ... 99 and starts again (0..9 use only the right digit) |
| `t_ir` | The OCCUPIED LED is on when something is in front of the IR sensor |
| `t_ultra` | The display shows the distance in cm. It shows `--` if the distance is above 99 cm or there is no echo |
| `t_pwm` | The buzzer sounds and changes tone strength every second (25%, 50%, 75%) |
| `t_adc` | Turning the knob changes the number on the display (0..93) |
| `t_gate` | If the IR sensor sees a car: `--` and the OCCUPIED LED. If not: the distance in cm |

`t_gate` is the Day-1 "gate" demo: ultrasonic + IR + display working together.

---

## 7. If something does not work

| Problem | Try this |
|---------|----------|
| `make: avr-gcc: command not found` | The tools are not installed, see step 5 |
| Upload says "permission denied" | Add yourself to `dialout` (step 5) and log in again |
| Upload says "can't open device" or timeouts | Check the port with `ls /dev/ttyACM* /dev/ttyUSB*` and pass it with `PORT=...`. Try another USB cable (some cables are charge-only) |
| An LED never lights | It may be backwards: the long leg goes to the resistor side, the short leg to GND |
| 7-segment shows nothing or wrong segments | Check that it is common cathode and that the transistors are wired as in the table |
| OCCUPIED LED is on when the slot is empty | Your IR module may output HIGH for "car there". The check in `drivers/ir.c` must be flipped |
| Distance is always `--` | Check TRIG/ECHO are not swapped and the sensor has 5V and GND |

Nothing here has been tested on a real board yet. Every test builds with
zero warnings.

---

## 8. For the curious: how it works inside

### Which timer does what

The chip has several timers. Each driver gets its own, so they never fight:

| Timer | Used by | Setup | Result |
|-------|---------|-------|--------|
| Timer0 | seg7 | CTC mode, clock / 256, OCR0A = 124 | interrupt every 2 ms, shows the next digit |
| Timer1 | timer | CTC mode, clock / 64, OCR1A = 249 | interrupt every 1 ms, counts milliseconds |
| Timer4 | pwm | fast PWM mode 14, clock / 8, ICR4 = 999 | 2 kHz signal on D6 for the buzzer |
| Timer5 | ultra | normal mode, clock / 8 | counts 0.5 us steps to time the echo |

The display has 2 digits but only one set of segment wires. Timer0 shows the
left digit, then the right digit, switching every 2 ms. It is so fast that
your eyes see both at once.

### No libraries at all

We do not use `<avr/io.h>` or any other avr-libc header. Every register is
written by its address from the datasheet (see `drivers/regs.h`).

- The datasheet shows two addresses for some registers, like
  `0x04 (0x24) DDRB`. The number in brackets (0x24) is the memory address,
  and that is the one C code must use.
- Interrupt functions are named `__vector_17` (Timer1) and `__vector_21`
  (Timer0). The datasheet numbers them 18 and 22, counting from 1. The
  compiler counts from 0, so we subtract 1.
- 16-bit registers are written as two bytes: high byte first, then the low
  byte. When reading, the low byte comes first.
- `avr-gcc` still adds its small startup code (sets up the stack and the
  interrupt table). That is part of the compiler, not a library.

### Register addresses used

Datasheet: Atmel ATmega2560, document 2549Q-AVR-02/2014.

| Register | Address | Datasheet page |
|----------|---------|----------------|
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

---

## Author

Ponmudi
