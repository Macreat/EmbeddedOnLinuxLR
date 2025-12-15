#include "sensor.h"
#include <pigpio.h>
#include <stdio.h>

#include "mq135.h"

static int mq135_pin = 23;
static int mq135_active_high = 1;

void mq135_init(int gpio_pin, int active_high)
{
    mq135_pin = gpio_pin;
    mq135_active_high = active_high;
    gpioSetMode(mq135_pin, PI_INPUT);
}

MQ135_Data mq135_read(void)
{
    MQ135_Data d = {0};
    if (mq135_pin < 0)
        return d;
    int raw = gpioRead(mq135_pin);
    d.triggered = mq135_active_high ? raw : !raw;
    return d;
}
