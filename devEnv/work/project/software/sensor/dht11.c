#include "sensor.h"
#include <stdio.h>

#include "dht11.h"
#include <pigpio.h>
#include <stdint.h>

static int dht_pin = -1;

void dht11_init(int gpio_pin)
{
    dht_pin = gpio_pin;
    gpioSetMode(dht_pin, PI_OUTPUT);
    gpioWrite(dht_pin, PI_HIGH);
    gpioDelay(500000); // 500 ms para estabilizar
}

DHT11_Data dht11_read(void)
{
    DHT11_Data result = {0};
    uint8_t bits[5] = {0};
    int last = PI_HIGH, counter = 0, j = 0;

    if (dht_pin < 0)
        return result;

    gpioSetMode(dht_pin, PI_OUTPUT);
    gpioWrite(dht_pin, PI_LOW);
    gpioDelay(18000); // start signal
    gpioWrite(dht_pin, PI_HIGH);
    gpioDelay(40);
    gpioSetMode(dht_pin, PI_INPUT);

    for (int i = 0; i < 85; i++)
    {
        counter = 0;
        while (gpioRead(dht_pin) == last)
        {
            counter++;
            gpioDelay(1);
            if (counter == 255)
                break;
        }
        last = gpioRead(dht_pin);
        if (counter == 255)
            break;
        if ((i >= 4) && (i % 2 == 0))
        {
            bits[j / 8] <<= 1;
            if (counter > 50)
                bits[j / 8] |= 1;
            j++;
        }
    }

    if (j >= 40)
    {
        result.hum = bits[0];
        result.temp_c = bits[2];
        result.valid = 1;
    }
    return result;
}
