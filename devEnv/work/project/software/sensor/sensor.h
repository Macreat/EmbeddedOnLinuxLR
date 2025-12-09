#ifndef SENSOR_H
#define SENSOR_H

// Initializes the sensor (opens file, seeds RNG, etc.)
void sensor_init(void);

// Reads and returns a single sensor value (simulated)
double sensor_read(void);

#endif