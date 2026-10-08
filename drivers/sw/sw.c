/*
 * sw.c - push switch driver (switch to GND)
 * Author: Ponmudi
 */

#include "board.h"
#include "gpio.h"
#include "sw.h"

/* set switch pin as input with pull-up */
void sw_init(void)
{
    gpio_dir(SW_PORT, SW_PIN, GPIO_IN_PULLUP);
}

/* returns 1 when pressed, 0 when not */
unsigned char sw_is_pressed(void)
{
    /* pressed pulls pin to 0 */
    if (gpio_get(SW_PORT, SW_PIN) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
