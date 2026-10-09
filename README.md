# Smart Parking Assist & Slot Monitoring (Arduino Mega 2560, bare-metal)

A **smart parking helper** for the 3-day bare-metal embedded hackathon. It
measures how far a car is from the wall, checks if a parking slot is free,
and shows this with LEDs, a number display, an LCD and a buzzer.

| Day | Work | Status |
|-----|------|--------|
| Day 1 | Drivers: gpio, sw, led, seg7, timer, pwm, adc, ir, ultra + one test each | Done, tested on the board |
| Day 2 | Parking app (`app/main.c`): state machine, zones, LEDs, 7-seg, buzzer | Done, tested on the board |
| Day 3 | Optional: LCD and keypad drivers, used by the app | Done, tested on the board |
| Day 3 | Optional, not done yet: external interrupt, servo barrier, timer input capture for ECHO | Open |

Quick links: [pin map](#4-parts-and-wiring), [the app](#the-parking-app),
[assumptions](#assumptions), [test results](#test-results-on-the-board).

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
app/main.c        the parking app (Day 2, LCD and keypad Day 3)
drivers/          all the drivers, one folder each
  regs.h          register addresses (from the datasheet), used by all drivers
  board.h         this project's wiring and options (only tests/app use it)
  gpio/           gpio.c/.h   pin control, all ports A..L
  led/            led.c/.h    the 5 status LEDs
  sw/             sw.c/.h     the push button
  timer/          timer.c/.h  time keeping: milliseconds and delays
  seg7/           seg7.c/.h   the 2-digit number display
  ir/             ir.c/.h     IR sensor: is the slot occupied?
  ultra/          ultra.c/.h  ultrasonic sensor: distance in cm
  pwm/            pwm.c/.h    PWM on D6/D7/D8 (buzzer sound)
  adc/            adc.c/.h    read a voltage on A0..A15 (IR #2 entry sensor)
  lcd/            lcd.c/.h    16x2 text display (JHD162A), 4-bit
  keypad/         keypad.c/.h 4x4 keypad
tests/            one small test program for each driver
Makefile          the build instructions (used by the `make` command)
```

### Suggested reading order

1. `drivers/board.h`: see where every part is connected in this project.
2. `drivers/gpio/gpio.c`: everything else is built on this.
3. `drivers/led/led.c` and `drivers/sw/sw.c`: the simplest drivers.
4. `drivers/timer/timer.c`: the first one with an interrupt.
5. `drivers/seg7/`, `drivers/ir/`, `drivers/ultra/`, `drivers/pwm/`, `drivers/adc/`.
6. Each test in `tests/`: short programs showing how to use a driver.

### Driver functions (the API)

The drivers are **general**: they do not know about this parking project.
You tell each driver which pins to use when you call its init function,
so you can use them in any ATmega2560 project. Only `drivers/board.h`
(this project's wiring) and the tests know where our parts are.

Drivers that can have more than one part (LED, switch, IR, ultrasonic)
give back an **id** from init. You keep the id and use it in later calls:

```c
unsigned char stop_led;
unsigned char key;

stop_led = led_init('A', 3);               /* LED on PA3 */
key = sw_init('E', 4, SW_ACTIVE_LOW);      /* switch on PE4 to GND */

if (sw_is_pressed(key))
{
    led_on(stop_led);
}
```

Wrong arguments (wrong port, pin, id, channel) never crash: the function
does nothing, or returns 0. A full table or wrong pin in init returns
`LED_NONE` / `SW_NONE` / `IR_NONE` / `ULTRA_NONE` (255).

**gpio**: port is `'A'`..`'L'` (no `'I'`), pin is 0..7. GPIO needs no init,
all pins are inputs after reset.

| Function | What it does |
|----------|--------------|
| `gpio_dir(port, pin, dir)` | `GPIO_IN`, `GPIO_OUT` or `GPIO_IN_PULLUP` |
| `gpio_set(port, pin)` / `gpio_clear(port, pin)` | pin high / low |
| `gpio_get(port, pin)` | read pin, 0 or 1 |
| `gpio_invert(port, pin)` | flip pin |

**led**: up to `LED_MAX` (8) LEDs, LED on = pin high.

| Function | What it does |
|----------|--------------|
| `led_init(port, pin)` | pin output, LED off, returns id |
| `led_on(id)` / `led_off(id)` / `led_toggle(id)` | one LED on / off / flip |
| `led_all_off()` | all added LEDs off |

**sw**: up to `SW_MAX` (4) switches.

| Function | What it does |
|----------|--------------|
| `sw_init(port, pin, type)` | `SW_ACTIVE_LOW` (switch to GND, pull-up on) or `SW_ACTIVE_HIGH` (switch to 5V, needs a pull-down resistor). Also starts the timer. Returns id |
| `sw_is_pressed(id)` | 1 while pressed right now (no debounce) |
| `sw_was_pressed(id)` | 1 once per press, 20 ms debounce. Call it often in the loop |

**timer**: Timer1, 1 ms tick.

| Function | What it does |
|----------|--------------|
| `timer_init()` | start the tick, interrupts on (safe to call twice) |
| `timer_millis()` | milliseconds since start |
| `timer_delay_ms(ms)` | wait (blocks) |
| `timer_elapsed(start, ms)` | 1 if `ms` passed since `start` (non-blocking timing) |

**seg7**: 2 digits, pos 0 = left (tens), pos 1 = right (ones). Timer0 refreshes it.

| Function | What it does |
|----------|--------------|
| `seg7_init(seg_port, d1_port, d1_pin, d2_port, d2_pin, type, digit_on)` | segments a..g,dp on pins 0..7 of `seg_port`, left digit on d1, right digit on d2. `type` = `SEG7_CATHODE` or `SEG7_ANODE`. `digit_on` = pin level that turns a digit on (1 if a transistor drives it). Starts refresh |
| `seg7_show_number(num)` | 0..99, 0..9 uses the right digit only, above 99 shows `--` |
| `seg7_show_digit(pos, value)` | 0..15 (0-9, A-F) on one digit |
| `seg7_show_dash()` | `--` |
| `seg7_blank()` | both digits off |
| `seg7_set_dp(pos, on)` | dot on/off |
| `seg7_show_raw(pos, pattern)` | own pattern, bit0 = a ... bit6 = g, bit7 = dot |

**ir**: IR obstacle sensor, up to `IR_MAX` (4).

| Function | What it does |
|----------|--------------|
| `ir_init(port, pin, type)` | `IR_ACTIVE_LOW` (output 0 when object seen, most modules) or `IR_ACTIVE_HIGH`. Returns id |
| `ir_read_raw(id)` | pin level now, 0 or 1 |
| `ir_is_detected(id)` | 1 only if 3 reads (~1 ms apart) all see an object |

**ultra**: HC-SR04, up to `ULTRA_MAX` (4), all share Timer5 (one measurement at a time).

| Function | What it does |
|----------|--------------|
| `ultra_init(trig_port, trig_pin, echo_port, echo_pin)` | pins, start Timer5, returns id |
| `ultra_get_us(id)` | echo time in us, `ULTRA_NO_ECHO_US` (65535) on timeout |
| `ultra_get_cm(id)` | distance in cm, `ULTRA_NO_ECHO` (999) on timeout |
| `ultra_get_cm_avg(id, n)` | average of n reads (1..8), 60 ms apart, timeouts skipped, 999 if all fail |

**pwm**: Timer4, channels `'A'` = D6, `'B'` = D7, `'C'` = D8. Default 2 kHz.

| Function | What it does |
|----------|--------------|
| `pwm_init(ch)` | pin output, start Timer4 (only the first time), output off |
| `pwm_set_freq(hz)` | 31..65535 Hz, shared by all 3 channels |
| `pwm_set_duty(ch, duty)` | 0..100 % |
| `pwm_on(ch)` / `pwm_off(ch)` | connect PWM to the pin / disconnect and pin low |

**adc**: AVcc (5V) reference, channels 0..15 = A0..A15.

| Function | What it does |
|----------|--------------|
| `adc_init()` | ADC on |
| `adc_read(ch)` | 0..1023 |
| `adc_read_avg(ch, n)` | average of n reads (1..16) |
| `adc_to_mv(raw)` | 0..1023 to 0..5000 mV |

**lcd**: 16x2 text LCD (HD44780 / JHD162A), 4-bit mode, write only (R/W to GND).
Uses short busy waits, no timer.

| Function | What it does |
|----------|--------------|
| `lcd_init(rs_port, rs_pin, e_port, e_pin, data_port, data_first_pin)` | DB4..DB7 on 4 pins in a row from `data_first_pin` (0..4). Runs the start sequence (about 60 ms) and clears. Returns 1, or 0 for wrong pins |
| `lcd_clear()` | clear, cursor to row 0, col 0 (about 2 ms) |
| `lcd_goto(row, col)` | row 0..1, col 0..15 |
| `lcd_putc(c)` / `lcd_print(text)` | one character / a text at the cursor |
| `lcd_print_number(num)` | 0..65535, no leading zeros |

**keypad**: 4x4 matrix keypad, keys `123A / 456B / 789C / *0#D`.

| Function | What it does |
|----------|--------------|
| `keypad_init(row_port, row_first_pin, col_port, col_first_pin, row_order)` | rows R1..R4 and columns L1..L4, each on 4 pins in a row (first pin 0..4). `row_order` = `KEYPAD_ROWS_NORMAL` (R1 = top row 1 2 3 A) or `KEYPAD_ROWS_REVERSED` (R1 = bottom row). Also starts the timer. Returns 1, or 0 for wrong pins |
| `keypad_get_key()` | key held now, or `KEYPAD_NO_KEY` (0) |
| `keypad_was_pressed()` | key once per press, 20 ms debounce, else `KEYPAD_NO_KEY`. Call it often in the loop |

---

## 4. Parts and wiring

The full sheet is `Smart_Parking_Pin_Connections.pdf`. You need: the Mega
board, a USB cable, a breadboard, jumper wires, 5 LEDs, 5 x 220 ohm (LEDs),
8 x 1 kohm (7-segment lines), a push button, two SUN056CC common-cathode
7-segment displays, an HC-SR04 ultrasonic sensor, two IR sensor modules and
a buzzer module. Day 3: a JHD162A 16x2 LCD and a 4x4 keypad.

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
| 7-seg segment a | D37 | PC0 | same pin of both displays on one wire, one 1 kohm resistor per line |
| 7-seg segment b | D36 | PC1 | |
| 7-seg segment c | D35 | PC2 | |
| 7-seg segment d | D34 | PC3 | |
| 7-seg segment e | D33 | PC4 | |
| 7-seg segment f | D32 | PC5 | |
| 7-seg segment g | D31 | PC6 | |
| 7-seg dot (dp) | D30 | PC7 | |
| 7-seg digit 1 (tens, left) | D41 | PG0 | left display COM (pin 3 or 8) straight to D41, no resistor |
| 7-seg digit 2 (ones, right) | D40 | PG1 | right display COM straight to D40 |
| Ultrasonic TRIG | D47 | PL2 | sensor VCC -> 5V, GND -> GND |
| Ultrasonic ECHO | D48 | PL1 | |
| IR #1 (slot) D0 | D46 | PL3 | sensor VCC -> 5V, GND -> GND |
| IR #2 (entry) A0 | A0 | PF0 (ADC0) | sensor VCC -> 5V, GND -> GND |
| Buzzer module S | D6 | PH3 (OC4A, PWM channel A) | middle -> 5V, - -> GND |
| LCD RS (4) | A8 | PK0 | LCD 1 VSS -> GND, 2 VCC -> 5V, 5 R/W -> GND |
| LCD E (6) | A9 | PK1 | 3 VEE -> about 1 kohm to GND, or pot middle (contrast) |
| LCD DB4..DB7 (11-14) | A12..A15 | PK4..PK7 | DB0..DB3 not connected. 15 LED+ -> 220 ohm -> 5V, 16 LED- -> GND |
| Keypad R1..R4 | D53, D52, D51, D50 | PB0..PB3 | |
| Keypad L1..L4 | D10, D11, D12, D13 | PB4..PB7 | the board's "L" LED (D13) may flicker while keys are read |

### This project's settings (board.h)

`drivers/board.h` holds this project's wiring. The tests pass these values
to the init functions. If your parts work differently, change these lines
(no driver code needs to change):

| Setting | Default | Meaning |
|---------|---------|---------|
| `SEG7_TYPE` | `SEG7_CATHODE` | or `SEG7_ANODE` for a common anode display |
| `SEG7_DIGIT_ON` | 0 | pin level that turns a digit on. 1 = through an NPN transistor, 0 = common cathode pin straight to the board |
| `IR_TYPE` | `IR_ACTIVE_LOW` | or `IR_ACTIVE_HIGH` if your IR module gives 1 when a car is there |
| `KEYPAD_ROW_ORDER` | `KEYPAD_ROWS_REVERSED` | our keypad's R1 wire is its bottom row. Use `KEYPAD_ROWS_NORMAL` if key 1 shows as `*` |

To move a part to another pin, change its `_PORT` / `_PIN` line in board.h.

---

## 5. Install the tools (Fedora Linux)

Open a terminal and run:

```
sudo dnf install avr-gcc avr-libc avrdude make
```

If the Arduino IDE is installed, its own copies also work. Put them on the
PATH for this terminal instead of installing:

```
export PATH=$HOME/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin:$HOME/.arduino15/packages/arduino/tools/avrdude/8.0.0-arduino1/bin:$PATH
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
| `t_ultra` | The display shows the distance in cm (average of 3 reads). It shows `--` if the distance is above 99 cm or there is no echo |
| `t_pwm` | The buzzer sounds and changes tone strength every second (25%, 50%, 75%) |
| `t_adc` | The display shows the IR #2 entry sensor value on A0 / 11 (0..93). Use it to set `ENTRY_ADC_LEVEL` in app/main.c |
| `t_gate` | If the IR sensor sees a car: `--` and the OCCUPIED LED. If not: the distance in cm |
| `t_lcd` | LCD line 1 shows `Smart Parking`, line 2 counts seconds |
| `t_keypad` | Each key you press is added to LCD line 2 and shown on the 7-segment (0-9, A-D, `--` for `#`). `*` clears the LCD |

`t_gate` is the Day-1 "gate" demo: ultrasonic + IR + display working together.

---

## 7. If something does not work

| Problem | Try this |
|---------|----------|
| `make: avr-gcc: command not found` | The tools are not installed, see step 5 |
| Upload says "permission denied" | Add yourself to `dialout` (step 5) and log in again |
| Upload says "can't open device" or timeouts | Check the port with `ls /dev/ttyACM* /dev/ttyUSB*` and pass it with `PORT=...`. Try another USB cable (some cables are charge-only) |
| An LED never lights | It may be backwards: the long leg goes to the resistor side, the short leg to GND |
| 7-segment shows nothing or wrong segments | Check both displays are common cathode (SUN056CC) and the COM pins go to D41 and D40. Check `SEG7_TYPE` and `SEG7_DIGIT_ON` in board.h |
| OCCUPIED LED is on when the slot is empty | Your IR module may output HIGH for "car there". Set `IR_TYPE` to `IR_ACTIVE_HIGH` in board.h |
| Distance is always `--` | Check TRIG/ECHO are not swapped and the sensor has 5V and GND |
| LCD lights up but shows nothing, or only boxes | Connect VEE (pin 3) for contrast, try another resistor or turn the pot. Check R/W (pin 5) is on GND |
| Wrong keypad keys | 1 shows as `*` (rows upside down): change `KEYPAD_ROW_ORDER` in board.h. Columns mixed up: L1..L4 go to D10..D13 |

---

## The parking app

`app/main.c` holds the state machine, the sensor decisions and the buzzer
patterns. It only calls driver functions, never registers.

| From | Condition | To |
|------|-----------|----|
| IDLE | IR #1 sees a car (slot full) | OCCUPIED |
| IDLE | IR #2 sees a car (entry) | MONITOR |
| OCCUPIED | IR #1 clear | IDLE |
| MONITOR | IR #1 sees a car | OCCUPIED |
| MONITOR | STOP zone for 3 s without a break | COMPLETE |
| MONITOR | IR #2 clear and distance > 99 cm for 2 s | IDLE |
| COMPLETE | distance > 99 cm (car left) | IDLE |
| any | push button or keypad `#` | IDLE |

| State | LCD line 1 | 7-segment | LEDs | Buzzer |
|-------|------------|-----------|------|--------|
| IDLE | `FREE` | `--` | all off | off |
| OCCUPIED | `OCCUPIED` | ` F` (full) | blue | off |
| MONITOR | `PARKING` + zone | distance | one zone LED | zone pattern |
| COMPLETE | `PARKED` | distance | red | off |

Zones (distance from the ultrasonic sensor, thresholds from the Day 2 brief):

| Distance | Zone | LED | Buzzer |
|----------|------|-----|--------|
| > 50 cm | SAFE | green | off |
| 31..50 cm | CAUTION | yellow | slow beep (100 ms on, 700 ms off) |
| 16..30 cm | WARNING | spare | fast beep (100 ms on, 200 ms off) |
| <= 15 cm | STOP | red | always on |

LCD line 2 shows `Dist: NN cm` (or `Dist: --`) and `MUTE` while muted.

Controls: push button or keypad `#` = back to IDLE from any state.
Keypad `*` = buzzer mute on/off.

Settings at the top of `app/main.c`: `SAFE_CM`, `CAUTION_CM`, `WARNING_CM`,
`ENTRY_ADC_LEVEL` (IR #2 threshold), `COMPLETE_MS`, `LEAVE_MS`, and
`USE_LCD` / `USE_KEYPAD` (set to 0 if that part is not wired).

---

## Assumptions

- Board: Arduino Mega 2560 (ATmega2560, 16 MHz), wired exactly as
  `Smart_Parking_Pin_Connections.pdf` (35 of 70 pins, no pin used twice).
- 7-segment: two SUN056CC **common cathode** displays, COM pins straight to
  D41/D40 (no transistors), 1 kohm per segment line.
- IR #1 (slot) is a digital module, **active low** (0 = object seen).
- IR #2 (entry) is read with the ADC on A0. Its value goes **low** when a car
  is near. Threshold 500 of 1023 (`ENTRY_ADC_LEVEL`), 50 counts hysteresis.
- Buzzer module is driven with PWM at 2 kHz, 50 % duty.
- The ultrasonic reading is filtered: the app uses the middle value of the
  last 3 good reads. 5 missed echoes in a row = no car (999 cm).
- "Parked" = STOP zone without a break for 3 s.
- Keypad: this 4x4 keypad's R1 wire is its **bottom** row, so board.h sets
  `KEYPAD_ROW_ORDER` to `KEYPAD_ROWS_REVERSED`.
- LCD: JHD162A in 4-bit mode, R/W tied to GND (write only, fixed delays
  instead of the busy flag).
- Ultrasonic ECHO is timed by polling Timer5, not input capture.

---

## Test results (on the board)

Tested on 2026-10-09 on the wired board (Mega 2560 clone, CH340, `/dev/ttyUSB0`).
Every program builds with zero warnings (`-Wall -Wextra`).

| Test | Result | Notes |
|------|--------|-------|
| `t_led_sw` | Pass | 5 LEDs one every 2 s, STOP LED follows the button |
| `t_timer` | Pass | 1 s blink |
| `t_seg7` | Pass | counts 0..99, tens left, ones right |
| `t_ir` | Pass | 0/1 and blue LED. A loose D26 wire was found and fixed |
| `t_ultra` | Pass | flickered with single reads, steady after averaging 3 reads |
| `t_pwm` | Pass | tone changes every second |
| `t_adc` | Pass | IR #2: high when clear, low when blocked |
| `t_gate` | Pass | Day 1 gate: distance when free, `--` + blue LED when occupied |
| `t_lcd` | Pass | needed VEE (contrast) wired to show text |
| `t_keypad` | Pass | after the row-order fix (key 1 used to read as `*`) |

Full app, step by step:

| Step | Result |
|------|--------|
| 1. Start: FREE, `--`, LEDs off, silent | Pass |
| 2. Block IR #2: PARKING, distance, green SAFE LED | Pass |
| 3. Move closer: CAUTION / WARNING / STOP with LED, LCD and buzzer | Pass |
| 4. STOP for 3 s: PARKED, buzzer off. Object away: FREE | Pass |
| 5. Block IR #1: OCCUPIED, blue LED, `F`. Clear: FREE | Pass |
| 6. `*` mutes, `#` and the push button reset to FREE | Pass |

---

## 8. For the curious: how it works inside

### Which timer does what

The chip has several timers. Each driver gets its own, so they never fight:

| Timer | Used by | Setup | Result |
|-------|---------|-------|--------|
| Timer0 | seg7 | CTC mode, clock / 256, OCR0A = 124 | interrupt every 2 ms, shows the next digit |
| Timer1 | timer | CTC mode, clock / 64, OCR1A = 249 | interrupt every 1 ms, counts milliseconds |
| Timer4 | pwm | fast PWM mode 14, clock / 8, top = ICR4 (999 = 2 kHz) | PWM on D6, D7, D8, same frequency for all |
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
| PINB/DDRB/PORTB | 0x23/0x24/0x25 | 96 |
| PINC/DDRC/PORTC | 0x26/0x27/0x28 | 97 |
| PIND/DDRD/PORTD | 0x29/0x2A/0x2B | 97 |
| PINE/DDRE/PORTE | 0x2C/0x2D/0x2E | 97-98 |
| PINF/DDRF/PORTF | 0x2F/0x30/0x31 | 97-98 |
| PING/DDRG/PORTG | 0x32/0x33/0x34 | 98 |
| PINH/DDRH/PORTH | 0x100/0x101/0x102 | 98-99 |
| PINJ/DDRJ/PORTJ | 0x103/0x104/0x105 | 99 |
| PINK/DDRK/PORTK | 0x106/0x107/0x108 | 99 |
| PINL/DDRL/PORTL | 0x109/0x10A/0x10B | 100 |
| SREG | 0x5F | 13 |
| TCCR0A / TCCR0B / OCR0A / TIMSK0 | 0x44 / 0x45 / 0x47 / 0x6E | 126-131 |
| TCCR1A / TCCR1B / OCR1AL-H / TIMSK1 | 0x80 / 0x81 / 0x88-0x89 / 0x6F | 154-161 |
| TCCR4A / TCCR4B / ICR4L-H | 0xA0 / 0xA1 / 0xA6-0xA7 | 154-161 |
| OCR4AL-H / OCR4BL-H / OCR4CL-H | 0xA8-0xA9 / 0xAA-0xAB / 0xAC-0xAD | 159-160 |
| TCCR5A / TCCR5B / TCNT5L-H | 0x120 / 0x121 / 0x124-0x125 | 154-158 |
| ADCL / ADCH / ADCSRA / ADCSRB / ADMUX | 0x78 / 0x79 / 0x7A / 0x7B / 0x7C | 281-287 |

---

## Author

Ponmudi
