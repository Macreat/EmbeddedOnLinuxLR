
#include <math.h>
#include "sensor.h"
#include <pigpio.h>
#include <stdio.h>

#include "mq2.h"

static int mq2_pin = 22;
static int mq2_active_high = 1;

void mq2_init(int gpio_pin, int active_high)
{
    mq2_pin = gpio_pin;
    mq2_active_high = active_high;
    gpioSetMode(mq2_pin, PI_INPUT);
}

MQ2_Data mq2_read(void)
{
    MQ2_Data d = {0};
    if (mq2_pin < 0)
        return d;
    int raw = gpioRead(mq2_pin);
    d.triggered = mq2_active_high ? raw : !raw;
    return d;
}
