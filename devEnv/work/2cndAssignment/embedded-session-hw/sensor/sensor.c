#include "sensor.h"
#include <pigpio.h>
#include <stdio.h> // <-- ADD THIS

#define SPI_CHANNEL 0

void sensor_init(void)
{
    if (gpioInitialise() < 0)
        perror("pigpio init failed");
}

double sensor_read(void)
{
    int handle = spiOpen(SPI_CHANNEL, 1000000, 0); // 1 MHz, mode 0
    char tx[] = {1, (8 + 0) << 4, 0};
    char rx[3];
    spiXfer(handle, tx, rx, 3);
    spiClose(handle);

    int value = ((rx[1] & 3) << 8) | rx[2];
    return (value / 1023.0) * 100.0;
}
