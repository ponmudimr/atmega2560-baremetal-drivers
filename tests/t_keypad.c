/*
 * t_keypad.c - each key is added to the LCD and shown on the 7-seg,
 *              '*' clears the LCD
 * Author: Ponmudi
 */

#include "board.h"
#include "keypad.h"
#include "lcd.h"
#include "seg7.h"

int main(void)
{
    char key;
    unsigned char col = 0;

    keypad_init(KEYPAD_ROW_PORT, KEYPAD_ROW_PIN, KEYPAD_COL_PORT, KEYPAD_COL_PIN);
    lcd_init(LCD_RS_PORT, LCD_RS_PIN, LCD_E_PORT, LCD_E_PIN, LCD_DATA_PORT, LCD_DATA_PIN);
    seg7_init(SEG7_SEG_PORT, SEG7_D1_PORT, SEG7_D1_PIN, SEG7_D2_PORT, SEG7_D2_PIN,
              SEG7_TYPE, SEG7_DIGIT_ON);

    lcd_print("Press a key");
    seg7_show_dash();

    while (1)
    {
        key = keypad_was_pressed();

        if (key == KEYPAD_NO_KEY)
        {
            continue;
        }

        /* 7-seg: 0-9 and A-D on the right digit, dash for * and # */
        seg7_show_raw(0, 0);   /* tens blank */
        if (key >= '0' && key <= '9')
        {
            seg7_show_digit(1, key - '0');
        }
        else if (key >= 'A' && key <= 'D')
        {
            seg7_show_digit(1, key - 'A' + 10);
        }
        else
        {
            seg7_show_dash();
        }

        /* LCD: '*' clears, other keys are added on line 2 */
        if (key == '*' || col >= LCD_COLS)
        {
            lcd_clear();
            col = 0;
        }
        if (key != '*')
        {
            lcd_goto(1, col);
            lcd_putc(key);
            col++;
        }
    }

    return 0;
}
