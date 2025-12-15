#include "led_actuator.h"
#include <pigpio.h>

static int r_pin = -1, g_pin = -1, b_pin = -1;
static int led_active_high = 1;

static void set_raw(int pin, int level)
{
    if (pin < 0)
        return;
    gpioWrite(pin, led_active_high ? level : !level);
}

void led_actuator_init(int r, int g, int b, int active_high)
{
    r_pin = r;
    g_pin = g;
    b_pin = b;
    led_active_high = active_high;
    gpioSetMode(r_pin, PI_OUTPUT);
    gpioSetMode(g_pin, PI_OUTPUT);
    gpioSetMode(b_pin, PI_OUTPUT);
    led_actuator_set(LED_OFF);
}

void led_actuator_set(LedColor color)
{
    int r = 0, g = 0, b = 0;
    switch (color)
    {
    case LED_RED:
        r = 1;
        break;
    case LED_GREEN:
        g = 1;
        break;
    case LED_BLUE:
        b = 1;
        break;
    case LED_YELLOW:
        r = 1;
        g = 1;
        break;
    case LED_CYAN:
        g = 1;
        b = 1;
        break;
    case LED_MAGENTA:
        r = 1;
        b = 1;
        break;
    case LED_WHITE:
        r = 1;
        g = 1;
        b = 1;
        break;
    case LED_OFF:
    default:
        break;
    }
    set_raw(r_pin, r);
    set_raw(g_pin, g);
    set_raw(b_pin, b);
}
