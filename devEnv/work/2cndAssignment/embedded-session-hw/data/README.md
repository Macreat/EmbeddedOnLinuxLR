# Data Inputs

This directory stores any sensor feeds or supporting datasets needed by the controller.

## Planned files
- `sensor_feed.csv` – primary replay list for the sensor module. Each line should contain a floating-point sample.
- Additional CSVs for stress tests (optional).

## Usage guidelines
1. Keep CSVs simple (one value per line) so `sensor.c` can parse them with `fscanf("%lf", ...)`.
2. Document provenance of each dataset in this README (e.g., “captured on Raspberry Pi 3B GPIO pin X at date/time Y”).
3. When random mode is used, note the seeding strategy in `sensor/README.md`.
4. Do not check in large binary datasets; keep inputs lightweight to support quick builds on WSL/Raspberry Pi.
