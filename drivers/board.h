/*
 * board.h - pin map and options for the parking board
 * Author: Ponmudi
 */

#ifndef BOARD_H
#define BOARD_H

/* LEDs, port A */
#define LED_PORT           'A'
#define LED_SAFE_PIN       0   /* PA0 = D22 */
#define LED_CAUTION_PIN    1   /* PA1 = D23 */
#define LED_WARNING_PIN    2   /* PA2 = D24 */
#define LED_STOP_PIN       3   /* PA3 = D25 */
#define LED_OCCUPIED_PIN   4   /* PA4 = D26 */

/* switch to GND, pull-up on */
#define SW_PORT            'E'
#define SW_PIN             4   /* PE4 = D2 */

/* 7-seg: a..g,dp = PC0..PC7 (D37..D30) */
#define SEG7_SEG_PORT      'C'
#define SEG7_DIGIT_PORT    'G'
#define SEG7_DIGIT1_PIN    0   /* PG0 = D41, tens */
#define SEG7_DIGIT2_PIN    1   /* PG1 = D40, ones */

/* ultrasonic */
#define ULTRA_PORT         'L'
#define ULTRA_TRIG_PIN     2   /* PL2 = D47 */
#define ULTRA_ECHO_PIN     1   /* PL1 = D48 */

/* IR sensor */
#define IR_PORT            'L'
#define IR_PIN             3   /* PL3 = D46 */

/* buzzer on OC4A */
#define BUZZER_PORT        'H'
#define BUZZER_PIN         3   /* PH3 = D6 */

/* pot on A0 */
#define POT_CHANNEL        0

/* options */
#define SEG7_COMMON_ANODE    0   /* 1 = common anode, segments inverted */
#define SEG7_DIGIT_ON_LEVEL  0   /* 0 = digit pin low turns it on, 1 = high (transistor) */
#define IR_ACTIVE_LOW        1   /* 1 = sensor gives 0 when car is there */

#endif
