#include "ky026.h"
#include "adc.h"
#include <pigpio.h>

static int ch;
static int d_pin;

void ky026_init(int adc_ch, int digital_pin)
{
    ch = adc_ch;
    d_pin = digital_pin;
    gpioSetMode(d_pin, PI_INPUT);
}

KY026_Data ky026_read()
{
    KY026_Data d;
    d.flame_detected = gpioRead(d_pin);
    d.flame_level = adc_read_channel(ch);
    return d;
}
