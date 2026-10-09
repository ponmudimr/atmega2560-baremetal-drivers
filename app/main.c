/*
 * main.c - Smart Parking Assist (Day 2): state machine, zones, LEDs,
 *          7-seg distance and buzzer, using only the driver APIs
 * Author: Ponmudi
 */

#include "board.h"
#include "timer.h"
#include "led.h"
#include "sw.h"
#include "seg7.h"
#include "ir.h"
#include "ultra.h"
#include "pwm.h"
#include "adc.h"
#include "lcd.h"
#include "keypad.h"

/* settings */
#define SAFE_CM            50     /* above this = SAFE */
#define CAUTION_CM         30     /* above this = CAUTION */
#define WARNING_CM         15     /* above this = WARNING, else STOP */

#define ENTRY_ADC_LEVEL    500    /* IR #2 level, calibrate with t_adc */
#define ENTRY_NEAR_IS_LOW  1      /* 1 = value goes low when car is near */
#define ENTRY_HYST         50     /* gap between detect and release */

#define COMPLETE_MS        3000   /* STOP this long = parked */
#define MEASURE_MS         100    /* ultrasonic period */
#define ADC_MS             100    /* entry sensor period */
#define LEAVE_MS           2000   /* no car this long = back to idle */
#define NO_ECHO_LIMIT      5      /* no-echos in a row = no car */
#define FAR_CM             999    /* distance used for "no car" */

#define USE_LCD            1      /* 0 = no LCD wired */
#define USE_KEYPAD         1      /* 0 = no keypad wired */
#define LCD_MS             250    /* LCD refresh period */
#define KEY_RESET          '#'    /* keypad: back to idle, like the switch */
#define KEY_MUTE           '*'    /* keypad: buzzer mute on/off */

/* zones */
#define ZONE_SAFE      0
#define ZONE_CAUTION   1
#define ZONE_WARNING   2
#define ZONE_STOP      3
#define ZONE_NONE      255        /* no zone LED, buzzer off */

/* states */
#define ST_IDLE        0
#define ST_OCCUPIED    1
#define ST_MONITOR     2
#define ST_COMPLETE    3

/* driver ids */
static unsigned char zone_led[4];   /* LED id for safe, caution, warning, stop */
static unsigned char occupied_led;
static unsigned char key;
static unsigned char slot_ir;
static unsigned char rear;

/* inputs */
static unsigned int distance = FAR_CM;   /* last good distance in cm */
static unsigned char no_echo_count = 0;
static unsigned int last_cm[3] = { FAR_CM, FAR_CM, FAR_CM };   /* last 3 good reads */
static unsigned char slot_full = 0;      /* 1 = IR #1 sees a car */
static unsigned char entry_seen = 0;     /* 1 = IR #2 sees a car */
static unsigned char zone = ZONE_SAFE;

/* state machine */
static unsigned char state = ST_IDLE;
static unsigned long stop_start;         /* when STOP zone began */
static unsigned long far_start;          /* when "no car" began */

/* timing */
static unsigned long last_measure;
static unsigned long last_adc;

/* buzzer */
static unsigned char buzzer_mode = ZONE_NONE;
static unsigned char buzzer_is_on = 0;
static unsigned long buzzer_time;
static unsigned char buzzer_muted = 0;   /* 1 = buzzer stays off */

/* keypad */
static unsigned char reset_request = 0;  /* 1 = '#' pressed */

#if USE_LCD
/* LCD text wanted now and text on the screen */
static char lcd_text[LCD_ROWS][LCD_COLS + 1];
static char lcd_shown[LCD_ROWS][LCD_COLS + 1];
static unsigned long last_lcd;
#endif

/* zone for a distance in cm */
static unsigned char get_zone(unsigned int cm)
{
    if (cm > SAFE_CM)
    {
        return ZONE_SAFE;
    }
    else if (cm > CAUTION_CM)
    {
        return ZONE_CAUTION;
    }
    else if (cm > WARNING_CM)
    {
        return ZONE_WARNING;
    }
    else
    {
        return ZONE_STOP;
    }
}

/* middle value of 3, so one wrong read is ignored */
static unsigned int median3(unsigned int a, unsigned int b, unsigned int c)
{
    if ((a >= b && a <= c) || (a <= b && a >= c))
    {
        return a;
    }
    else if ((b >= a && b <= c) || (b <= a && b >= c))
    {
        return b;
    }
    else
    {
        return c;
    }
}

/* measure distance every MEASURE_MS, keep last good value on no echo */
static void read_distance(void)
{
    unsigned int cm;

    if (!timer_elapsed(last_measure, MEASURE_MS))
    {
        return;
    }
    last_measure = timer_millis();

    cm = ultra_get_cm(rear);

    if (cm == ULTRA_NO_ECHO)
    {
        if (no_echo_count < NO_ECHO_LIMIT)
        {
            no_echo_count++;
        }
        if (no_echo_count >= NO_ECHO_LIMIT)
        {
            distance = FAR_CM;   /* too many misses, no car */
        }
    }
    else
    {
        no_echo_count = 0;
        last_cm[0] = last_cm[1];
        last_cm[1] = last_cm[2];
        last_cm[2] = cm;
        distance = median3(last_cm[0], last_cm[1], last_cm[2]);
    }
}

/* read IR #2 on the ADC every ADC_MS, with hysteresis */
static void read_entry(void)
{
    unsigned int raw;

    if (!timer_elapsed(last_adc, ADC_MS))
    {
        return;
    }
    last_adc = timer_millis();

    raw = adc_read(ENTRY_ADC_CHANNEL);

    if (ENTRY_NEAR_IS_LOW)
    {
        if (raw < ENTRY_ADC_LEVEL)
        {
            entry_seen = 1;
        }
        else if (raw > ENTRY_ADC_LEVEL + ENTRY_HYST)
        {
            entry_seen = 0;
        }
    }
    else
    {
        if (raw > ENTRY_ADC_LEVEL)
        {
            entry_seen = 1;
        }
        else if (raw < ENTRY_ADC_LEVEL - ENTRY_HYST)
        {
            entry_seen = 0;
        }
    }
}

#if USE_KEYPAD
/* '#' asks for a reset, '*' turns the buzzer mute on/off */
static void read_keypad(void)
{
    char k = keypad_was_pressed();

    if (k == KEY_RESET)
    {
        reset_request = 1;
    }
    else if (k == KEY_MUTE)
    {
        buzzer_muted = !buzzer_muted;
    }
}
#endif

/* read all sensors */
static void read_inputs(void)
{
    slot_full = ir_is_detected(slot_ir);
    read_distance();
    read_entry();
    zone = get_zone(distance);
#if USE_KEYPAD
    read_keypad();
#endif
}

/* go to a new state and restart its timers */
static void change_state(unsigned char new_state)
{
    state = new_state;
    stop_start = timer_millis();
    far_start = timer_millis();
}

/* decide the next state */
static void update_state(void)
{
    /* switch or '#': back to idle from any state */
    if (sw_was_pressed(key) || reset_request)
    {
        reset_request = 0;
        change_state(ST_IDLE);
        return;
    }

    switch (state)
    {
        case ST_IDLE:
            if (slot_full)
            {
                change_state(ST_OCCUPIED);
            }
            else if (entry_seen)
            {
                change_state(ST_MONITOR);
            }
            break;

        case ST_OCCUPIED:
            if (!slot_full)
            {
                change_state(ST_IDLE);
            }
            break;

        case ST_MONITOR:
            if (slot_full)
            {
                change_state(ST_OCCUPIED);
                break;
            }

            /* car gone: no entry and far for LEAVE_MS */
            if (!entry_seen && distance > 99)
            {
                if (timer_elapsed(far_start, LEAVE_MS))
                {
                    change_state(ST_IDLE);
                    break;
                }
            }
            else
            {
                far_start = timer_millis();
            }

            /* parked: STOP zone for COMPLETE_MS without a break */
            if (zone == ZONE_STOP)
            {
                if (timer_elapsed(stop_start, COMPLETE_MS))
                {
                    change_state(ST_COMPLETE);
                }
            }
            else
            {
                stop_start = timer_millis();
            }
            break;

        case ST_COMPLETE:
            if (distance > 99)
            {
                change_state(ST_IDLE);
            }
            break;

        default:
            change_state(ST_IDLE);   /* should not happen */
            break;
    }
}

/* one zone LED on (or none), others off */
static void set_zone_led(unsigned char z)
{
    unsigned char i;

    for (i = 0; i < 4; i++)
    {
        if (i == z)
        {
            led_on(zone_led[i]);
        }
        else
        {
            led_off(zone_led[i]);
        }
    }
}

/* distance on 7-seg, dash if more than 99 */
static void show_distance(void)
{
    if (distance > 99)
    {
        seg7_show_dash();
    }
    else
    {
        seg7_show_number((unsigned char)distance);
    }
}

/* set LEDs, 7-seg and buzzer mode for the state */
static void update_outputs(void)
{
    switch (state)
    {
        case ST_IDLE:
            set_zone_led(ZONE_NONE);
            led_off(occupied_led);
            seg7_show_dash();
            buzzer_mode = ZONE_NONE;
            break;

        case ST_OCCUPIED:
            set_zone_led(ZONE_NONE);
            led_on(occupied_led);
            seg7_show_raw(0, 0);           /* tens blank (no segments) */
            seg7_show_digit(1, 15);        /* "F" = full */
            buzzer_mode = ZONE_NONE;
            break;

        case ST_MONITOR:
            set_zone_led(zone);
            led_off(occupied_led);
            show_distance();
            buzzer_mode = zone;
            break;

        case ST_COMPLETE:
            set_zone_led(ZONE_STOP);
            led_off(occupied_led);
            show_distance();
            buzzer_mode = ZONE_NONE;
            break;

        default:
            break;
    }
}

/* buzzer on (1) or off (0), only call pwm when it changes */
static void buzzer_set(unsigned char on)
{
    if (on == buzzer_is_on)
    {
        return;
    }

    if (on)
    {
        pwm_on(BUZZER_PWM_CH);
    }
    else
    {
        pwm_off(BUZZER_PWM_CH);
    }
    buzzer_is_on = on;
    buzzer_time = timer_millis();
}

/* beep: on for on_ms, off for off_ms, again and again */
static void buzzer_beep(unsigned int on_ms, unsigned int off_ms)
{
    if (buzzer_is_on)
    {
        if (timer_elapsed(buzzer_time, on_ms))
        {
            buzzer_set(0);
        }
    }
    else
    {
        if (timer_elapsed(buzzer_time, off_ms))
        {
            buzzer_set(1);
        }
    }
}

/* buzzer pattern for buzzer_mode, called every loop */
static void buzzer_update(void)
{
    if (buzzer_muted)
    {
        buzzer_set(0);
        return;
    }

    switch (buzzer_mode)
    {
        case ZONE_CAUTION:
            buzzer_beep(100, 700);   /* slow beep */
            break;

        case ZONE_WARNING:
            buzzer_beep(100, 200);   /* fast beep */
            break;

        case ZONE_STOP:
            buzzer_set(1);           /* always on */
            break;

        default:
            buzzer_set(0);           /* SAFE or off */
            break;
    }
}

#if USE_LCD
/* write text into an LCD line from col, cut at the line end */
static void lcd_text_put(char *line, unsigned char col, const char *text)
{
    while (*text != '\0' && col < LCD_COLS)
    {
        line[col] = *text;
        col++;
        text++;
    }
}

/* number of characters in text */
static unsigned char text_len(const char *text)
{
    unsigned char n = 0;

    while (text[n] != '\0')
    {
        n++;
    }
    return n;
}

/* 1 if two LCD lines are the same */
static unsigned char lcd_line_same(const char *a, const char *b)
{
    unsigned char i;

    for (i = 0; i < LCD_COLS; i++)
    {
        if (a[i] != b[i])
        {
            return 0;
        }
    }
    return 1;
}

/* make the 2 LCD lines for the state */
static void lcd_make_text(void)
{
    static const char *state_name[4] = { "FREE", "OCCUPIED", "PARKING", "PARKED" };
    static const char *zone_name[4] = { "SAFE", "CAUTION", "WARNING", "STOP" };
    char number[3];
    unsigned char row;
    unsigned char i;

    for (row = 0; row < LCD_ROWS; row++)
    {
        for (i = 0; i < LCD_COLS; i++)
        {
            lcd_text[row][i] = ' ';
        }
        lcd_text[row][LCD_COLS] = '\0';
    }

    /* line 1: state, zone on the right while parking */
    if (state <= ST_COMPLETE)
    {
        lcd_text_put(lcd_text[0], 0, state_name[state]);
    }
    if (state == ST_MONITOR && zone <= ZONE_STOP)
    {
        lcd_text_put(lcd_text[0], LCD_COLS - text_len(zone_name[zone]), zone_name[zone]);
    }

    /* line 2: distance, MUTE on the right */
    lcd_text_put(lcd_text[1], 0, "Dist:");
    if (distance > 99)
    {
        lcd_text_put(lcd_text[1], 6, "--");
    }
    else
    {
        number[0] = '0' + distance / 10;
        number[1] = '0' + distance % 10;
        number[2] = '\0';
        if (number[0] == '0')
        {
            number[0] = ' ';   /* no leading zero */
        }
        lcd_text_put(lcd_text[1], 6, number);
        lcd_text_put(lcd_text[1], 9, "cm");
    }
    if (buzzer_muted)
    {
        lcd_text_put(lcd_text[1], 12, "MUTE");
    }
}

/* every LCD_MS, write only the lines that changed */
static void lcd_update(void)
{
    unsigned char row;
    unsigned char i;

    if (!timer_elapsed(last_lcd, LCD_MS))
    {
        return;
    }
    last_lcd = timer_millis();

    lcd_make_text();

    for (row = 0; row < LCD_ROWS; row++)
    {
        if (!lcd_line_same(lcd_text[row], lcd_shown[row]))
        {
            lcd_goto(row, 0);
            lcd_print(lcd_text[row]);
            for (i = 0; i <= LCD_COLS; i++)
            {
                lcd_shown[row][i] = lcd_text[row][i];
            }
        }
    }
}
#endif

int main(void)
{
    timer_init();

    zone_led[ZONE_SAFE] = led_init(LED_SAFE_PORT, LED_SAFE_PIN);
    zone_led[ZONE_CAUTION] = led_init(LED_CAUTION_PORT, LED_CAUTION_PIN);
    zone_led[ZONE_WARNING] = led_init(LED_WARNING_PORT, LED_WARNING_PIN);
    zone_led[ZONE_STOP] = led_init(LED_STOP_PORT, LED_STOP_PIN);
    occupied_led = led_init(LED_OCCUPIED_PORT, LED_OCCUPIED_PIN);

    key = sw_init(SW_PORT, SW_PIN, SW_ACTIVE_LOW);
    slot_ir = ir_init(IR_PORT, IR_PIN, IR_TYPE);
    rear = ultra_init(ULTRA_TRIG_PORT, ULTRA_TRIG_PIN, ULTRA_ECHO_PORT, ULTRA_ECHO_PIN);
    seg7_init(SEG7_SEG_PORT, SEG7_D1_PORT, SEG7_D1_PIN, SEG7_D2_PORT, SEG7_D2_PIN,
              SEG7_TYPE, SEG7_DIGIT_ON);
    adc_init();
#if USE_LCD
    lcd_init(LCD_RS_PORT, LCD_RS_PIN, LCD_E_PORT, LCD_E_PIN, LCD_DATA_PORT, LCD_DATA_PIN);
#endif
#if USE_KEYPAD
    keypad_init(KEYPAD_ROW_PORT, KEYPAD_ROW_PIN, KEYPAD_COL_PORT, KEYPAD_COL_PIN,
                KEYPAD_ROW_ORDER);
#endif

    pwm_init(BUZZER_PWM_CH);              /* 2 kHz, output off */
    pwm_set_duty(BUZZER_PWM_CH, 50);

    last_measure = timer_millis();
    last_adc = timer_millis();
#if USE_LCD
    last_lcd = timer_millis() - LCD_MS;   /* first update at once */
#endif
    change_state(ST_IDLE);

    while (1)
    {
        read_inputs();
        update_state();
        update_outputs();
        buzzer_update();
#if USE_LCD
        lcd_update();
#endif
    }

    return 0;
}
