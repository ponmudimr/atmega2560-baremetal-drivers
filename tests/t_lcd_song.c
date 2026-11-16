/*
 * t_lcd_song.c - LCD line 1 song name, line 2 scrolls the lyrics
 * Song: Chinnanjiru Kiliye, by Bharathiyar (public domain), Tamil in English letters
 * Author: Ponmudi
 */

#include "board.h"
#include "timer.h"
#include "lcd.h"

#define SCROLL_MS  300   /* move the text one step every 300 ms */

/* 16 spaces at the end, so the text leaves the screen before it starts again */
static const char song[] =
    "Chinnanjiru kiliye, kannamma! Selva kalanjiyame! "
    "Ennai kali theerthe, ulagil etram puriya vandhaai!"
    "                ";

int main(void)
{
    unsigned int len = 0;
    unsigned int pos = 0;
    unsigned int idx;
    unsigned char i;
    unsigned long last;

    timer_init();
    lcd_init(LCD_RS_PORT, LCD_RS_PIN, LCD_E_PORT, LCD_E_PIN, LCD_DATA_PORT, LCD_DATA_PIN);

    /* count the characters of the song */
    while (song[len] != '\0')
    {
        len++;
    }

    lcd_goto(0, 0);
    lcd_print("Bharathiyar song");

    last = timer_millis();

    while (1)
    {
        if (timer_elapsed(last, SCROLL_MS))
        {
            last = timer_millis();

            /* show 16 characters, starting at pos, wrap at the end */
            lcd_goto(1, 0);
            for (i = 0; i < LCD_COLS; i++)
            {
                idx = pos + i;
                if (idx >= len)
                {
                    idx = idx - len;
                }
                lcd_putc(song[idx]);
            }

            pos++;
            if (pos >= len)
            {
                pos = 0;
            }
        }
    }

    return 0;
}
