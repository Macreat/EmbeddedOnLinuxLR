#ifndef LED_ACTUATOR_H
#define LED_ACTUATOR_H

typedef enum
{
    LED_OFF = 0,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_YELLOW,
    LED_CYAN,
    LED_MAGENTA,
    LED_WHITE
} LedColor;

void led_actuator_init(int r_pin, int g_pin, int b_pin, int active_high); // active_high=1 cátodo común
void led_actuator_set(LedColor color);

#endif
