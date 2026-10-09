/*
 * board.h - this project's wiring and options, used by tests/app only
 *           (drivers do not include it, they get pins from init)
 * Author: Ponmudi
 */

#ifndef BOARD_H
#define BOARD_H

/* LED pins */
#define LED_SAFE_PORT      'A'
#define LED_SAFE_PIN       0   /* PA0 = D22 */
#define LED_CAUTION_PORT   'A'
#define LED_CAUTION_PIN    1   /* PA1 = D23 */
#define LED_WARNING_PORT   'A'
#define LED_WARNING_PIN    2   /* PA2 = D24 */
#define LED_STOP_PORT      'A'
#define LED_STOP_PIN       3   /* PA3 = D25 */
#define LED_OCCUPIED_PORT  'A'
#define LED_OCCUPIED_PIN   4   /* PA4 = D26 */

/* push switch to GND (active low) */
#define SW_PORT            'E'
#define SW_PIN             4   /* PE4 = D2 */

/* 7-seg: a..g,dp = PC0..PC7 (D37..D30) */
#define SEG7_SEG_PORT      'C'
#define SEG7_D1_PORT       'G'
#define SEG7_D1_PIN        0   /* PG0 = D41, tens */
#define SEG7_D2_PORT       'G'
#define SEG7_D2_PIN        1   /* PG1 = D40, ones */
#define SEG7_TYPE          SEG7_CATHODE   /* or SEG7_ANODE, from seg7.h */
#define SEG7_DIGIT_ON      0   /* 0 = digit on when pin low (COM pin direct) */

/* ultrasonic */
#define ULTRA_TRIG_PORT    'L'
#define ULTRA_TRIG_PIN     2   /* PL2 = D47 */
#define ULTRA_ECHO_PORT    'L'
#define ULTRA_ECHO_PIN     1   /* PL1 = D48 */

/* IR sensor */
#define IR_PORT            'L'
#define IR_PIN             3   /* PL3 = D46 */
#define IR_TYPE            IR_ACTIVE_LOW   /* or IR_ACTIVE_HIGH, from ir.h */

/* buzzer on PWM channel A */
#define BUZZER_PWM_CH      'A' /* OC4A = PH3 = D6 */

/* pot on A0 */
#define POT_CHANNEL        0

/* IR #2 entry sensor, analog out on A0 */
#define ENTRY_ADC_CHANNEL  0

/* LCD JHD162A, 4-bit, R/W to GND */
#define LCD_RS_PORT        'K'
#define LCD_RS_PIN         0   /* PK0 = A8 */
#define LCD_E_PORT         'K'
#define LCD_E_PIN          1   /* PK1 = A9 */
#define LCD_DATA_PORT      'K'
#define LCD_DATA_PIN       4   /* PK4..PK7 = A12..A15, DB4..DB7 */

/* 4x4 keypad */
#define KEYPAD_ROW_PORT    'B'
#define KEYPAD_ROW_PIN     0   /* PB0..PB3 = D53..D50, R1..R4 */
#define KEYPAD_COL_PORT    'B'
#define KEYPAD_COL_PIN     4   /* PB4..PB7 = D10..D13, L1..L4 */
#define KEYPAD_ROW_ORDER   KEYPAD_ROWS_REVERSED   /* this keypad: R1 = bottom row, from keypad.h */

#endif
