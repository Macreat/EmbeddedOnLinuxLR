#include "sensor.h"
#include <stdio.h>

static FILE *f = NULL; // file handle for sensor data

void sensor_init(void)
{
    f = fopen("./data/sensor_feed.csv", "r"); // open CSV file with simulated readings
    if (!f)
        perror("error opening sensor_feed.csv"); // show error if not found
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
}
