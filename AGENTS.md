# AGENTS.md - rules and handoff for AI coding agents

Project: Smart Parking Assist & Slot Monitoring, ATmega2560 (Arduino Mega 2560),
bare-metal Embedded C, 3-day hackathon. Author: Ponmudi.
Read `README.md` for the full pin map, driver API, app behaviour and test results.

## Hackathon rules (must not be broken)

- Bare-metal, register-level C only. **No Arduino libraries, no avr-libc
  headers** (`<avr/io.h>` etc.), no ready-made LCD/keypad/ultrasonic libraries.
  Registers are written by address from `drivers/regs.h` (datasheet 2549Q).
- Layered architecture: `app/` and `tests/` -> driver API -> registers -> hardware.
  `app/main.c` and `tests/*.c` **never** touch registers (DDRx, PORTx, PINx,
  ADCSRA, TCCRn, OCRn, ...). They only call driver functions.
- `app/main.c` holds the state machine, sensor decisions and buzzer patterns.
  Driver `.c` files hold the register code. Driver `.h` files expose only the API.
- Optional features never replace mandatory ones.

## Code rules

- Drivers are generic: pins come from the init function. Drivers never
  include `board.h`. Only `board.h` (and tests/app) know this project's wiring.
- Wrong arguments never crash: init returns `X_NONE` (255) or 0, other calls
  ignore bad ids/pins.
- Style: Allman braces, 4 spaces, `unsigned char` / `unsigned int`, short
  `/* */` comments in simple English, file header with `Author: Ponmudi`.
- Short waits in drivers that do not own a timer: `volatile` busy loop
  (see `ir_wait_1ms()` in `drivers/ir/ir.c`).
- ISR names: `__vector_N` with `__attribute__((signal, used, externally_visible))`,
  N = datasheet vector number - 1.
- New driver = new folder `drivers/<name>/<name>.c/.h`. The Makefile finds it by itself.
- Builds must have zero warnings (`-Wall -Wextra`).
- Git: never add AI co-author lines or "generated with" footers to commits or PRs.

## Build and flash

The board is on `/dev/ttyUSB0` (CH340 clone). avr-gcc/avrdude are not on the
system PATH. Use the Arduino IDE copies:

```
export PATH=$HOME/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin:$HOME/.arduino15/packages/arduino/tools/avrdude/8.0.0-arduino1/bin:$PATH
make TEST=t_seg7                              # build one test
make flash TEST=t_seg7 PORT=/dev/ttyUSB0      # build + upload a test
make flash-app PORT=/dev/ttyUSB0              # build + upload the app
make clean
```

## Hardware (all wired, see Smart_Parking_Pin_Connections.pdf / README section 4)

| Part | Pins |
|------|------|
| LEDs SAFE/CAUTION/WARNING/STOP/OCCUPIED | PA0..PA4 = D22..D26 |
| Push button (to GND) | PE4 = D2 |
| 7-seg x2 common cathode, segments a..dp | PC0..PC7 = D37..D30, COM tens PG0 = D41, ones PG1 = D40 |
| Ultrasonic HC-SR04 | TRIG PL2 = D47, ECHO PL1 = D48 (Timer5) |
| IR #1 slot (digital, active low) | PL3 = D46 |
| IR #2 entry (analog) | PF0 = A0 (ADC ch 0) |
| Buzzer module | PH3 / OC4A = D6 (Timer4 PWM) |
| LCD JHD162A 4-bit | RS PK0 = A8, E PK1 = A9, DB4..DB7 PK4..PK7 = A12..A15 |
| Keypad 4x4 | rows PB0..PB3 = D53..D50 (reversed: R1 = bottom row), cols PB4..PB7 = D10..D13 |

Timers: Timer0 seg7 refresh (2 ms ISR), Timer1 1 ms tick (ISR), Timer4 buzzer
PWM, Timer5 ultrasonic echo timing (polled). **Timer3 is free** (servo).
Free pins: 35 of 70, for example port D (D18..D21, INT0..INT3 for an external
interrupt) and PE5 = D3 (INT5).

## Status (2026-10-09)

Everything below was tested on the real board and passes:
- Day 1: all 9 drivers + tests `t_led_sw t_timer t_seg7 t_ir t_ultra t_pwm t_adc t_gate`.
- Day 2: full app, all 6 app steps (README "Test results").
- Day 3 optional: LCD (`t_lcd`) and keypad (`t_keypad`) drivers, used by the app.

## Open work (Day 3 optional challenges, not started)

1. **External interrupt** (2 marks): emergency/override switch on an INTn pin.
   ISR only sets a flag, the app does the work. New driver `drivers/exti/`.
   Note: the push button is already on PE4 = D2, which is INT4, so it can be used
   without rewiring. Otherwise use INT0..INT3 (D21..D18) or INT5 (D3).
2. **Servo/motor barrier** with PWM: Timer3 (free) at 50 Hz, OC3A = PE3 = D5. Barrier opens when
   the slot is available, stays closed when OCCUPIED.
3. **Timer input capture for ECHO**: ICP5 is PL1 = D48, which is already the
   ECHO pin, so `ultra` could use Timer5 input capture instead of polling.
4. Optional extra app modes via the keypad (for example threshold setting).

Before adding a part, pick free pins that do not clash (check README pin table),
add them to `drivers/board.h`, write a test in `tests/`, then use it in the app.
Update README (API table, wiring, tests table, test results) with every change.
