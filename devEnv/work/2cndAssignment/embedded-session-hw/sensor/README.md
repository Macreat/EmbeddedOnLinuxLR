# Sensor Blueprint

Implements `sensor.h` and `sensor.c`. Provides an abstraction for light sensing using either simulation or hardware SPI.

## Key functions

- `void sensor_init(void);`
- `double sensor_read(void);` // returns 0–100%

## Modes

- **Simulation:** replay CSV from `../data/sensor_feed.csv`.
- **Hardware:** read from MCP3008 over SPI (via `pigpio`).

## Notes

- Guard all headers with `#ifndef SENSOR_H`.
- Return percentage scaled from ADC (10-bit → 0–100).
- Keep state (file handle or SPI handle) static within `sensor.c`.
