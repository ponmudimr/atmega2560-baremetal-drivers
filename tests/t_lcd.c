/*
 * t_lcd.c - LCD line 1 "Smart Parking", line 2 counts seconds
 * Author: Ponmudi
 */

#include "board.h"
#include "timer.h"
#include "lcd.h"

int main(void)
{
    unsigned int seconds = 0;
    unsigned long last;

    timer_init();
    lcd_init(LCD_RS_PORT, LCD_RS_PIN, LCD_E_PORT, LCD_E_PIN, LCD_DATA_PORT, LCD_DATA_PIN);

    lcd_goto(0, 0);
    lcd_print("Smart Parking");
    lcd_goto(1, 0);
    lcd_print("Time: 0 s");

    last = timer_millis();

    while (1)
    {
        if (timer_elapsed(last, 1000))
        {
            last = timer_millis();
            seconds++;

            lcd_goto(1, 6);
            lcd_print_number(seconds);
            lcd_print(" s");
        }
    }

    return 0;
}
