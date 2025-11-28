#include "dht11.h"
#include <pigpio.h>
#include <stdio.h>

static int dht_pin;

void dht11_init(int gpio_pin)
{
    dht_pin = gpio_pin;
    gpioSetMode(dht_pin, PI_OUTPUT);
    gpioWrite(dht_pin, PI_HIGH);
    gpioDelay(500000);
}

DHT11_Data dht11_read()
{
    DHT11_Data result = {0};
    uint8_t bits[5] = {0};

    int last = PI_HIGH;
    int counter = 0;
    int j = 0;

    gpioSetMode(dht_pin, PI_OUTPUT);
    gpioWrite(dht_pin, PI_LOW);
    gpioDelay(18000);

    gpioWrite(dht_pin, PI_HIGH);
    gpioDelay(40);

    gpioSetMode(dht_pin, PI_INPUT);

    for (int i = 0; i < 85; i++) {
        counter = 0;
        while (gpioRead(dht_pin) == last) {
            counter++;
            gpioDelay(1);
            if (counter == 255) break;
        }
        last = gpioRead(dht_pin);

        if (counter == 255) break;
        if ((i >= 4) && (i % 2 == 0)) {
            bits[j / 8] <<= 1;
            if (counter > 50) bits[j / 8] |= 1;
            j++;
        }
    }

    if (j >= 40) {
        result.hum = bits[0];
        result.temp = bits[2];
        result.valid = 1;
    } else {
        result.valid = 0;
    }

    return result;
}
