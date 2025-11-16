#include <stdio.h>
#include "../sensor/sensor.h"

// Test program: reads 10 values from the simulated sensor
int main(void)
{
    sensor_init(); // initialize the sensor (open data file)
    for (int i = 0; i < 10; i++)
    {
        double value = sensor_read(); // get one reading
        printf("Reading %d: %.2f\n", i + 1, value);
    }
    return 0;
}
