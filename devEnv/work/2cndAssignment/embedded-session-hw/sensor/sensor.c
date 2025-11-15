#include "sensor.h"
#include <pigpio.h>
#include <stdio.h>

#define BUTTON_PIN 22 // GPIO 22 (pin 15 on the board)

void sensor_init(void)
{
    if (gpioInitialise() < 0)
        perror("pigpio init failed");

    gpioSetMode(BUTTON_PIN, PI_INPUT);
    gpioSetPullUpDown(BUTTON_PIN, PI_PUD_DOWN); // enable internal pull-down
}

double sensor_read(void)
{
    int state = gpioRead(BUTTON_PIN); // 1 = pressed, 0 = released

    if (state)
        return 10.0; // simulate "dark" → low light
    else
        return 90.0; // simulate "bright" → high light
}
