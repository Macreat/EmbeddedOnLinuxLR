#include "sensor.h"
#include <pigpio.h>
#include <stdio.h>

#include "ky026.h"

static int flame_pin = 27;
static int flame_active_high = 0; // muchos KY-026 entregan LOW al detectar

void ky026_init(int gpio_pin, int active_high)
{
    flame_pin = gpio_pin;
    flame_active_high = active_high;
    gpioSetMode(flame_pin, PI_INPUT);
}

KY026_Data ky026_read(void)
{
    KY026_Data d = {0};
    if (flame_pin < 0)
        return d;
    int raw = gpioRead(flame_pin);
    d.flame_detected = flame_active_high ? raw : !raw;
    return d;
}
