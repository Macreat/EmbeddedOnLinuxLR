#include "sensor.h"
#include <stdio.h>
#include <pigpio.h>
#include <stdio.h> // <-- ADD THIS

static FILE *f = NULL; // file handle for sensor data
#define SPI_CHANNEL 0

void sensor_init(void)
{
    f = fopen("./data/sensor_feed.csv", "r"); // open CSV file with simulated readings
    if (!f)
        perror("error opening sensor_feed.csv"); // show error if not found
    if (gpioInitialise() < 0)
        perror("pigpio init failed");
}

double sensor_read(void)
{
    if (!f)
        return 0.0; // return 0 if file not open
    double val;
    if (fscanf(f, "%lf", &val) == 1) // read one numeric value
        return val;
    else
    {
        rewind(f); // restart file if end reached
        return 0.0;
    }
    int handle = spiOpen(SPI_CHANNEL, 1000000, 0); // 1 MHz, mode 0
    char tx[] = {1, (8 + 0) << 4, 0};
    char rx[3];
    spiXfer(handle, tx, rx, 3);
    spiClose(handle);

    int value = ((rx[1] & 3) << 8) | rx[2];
    return (value / 1023.0) * 100.0;
}