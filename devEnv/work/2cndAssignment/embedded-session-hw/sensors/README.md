# Sensor Module Checklist

Goal: provide `sensor.h`/`sensor.c` that expose `sensor_init(void)` and `sensor_read(void)` with clean separation of declarations and definitions.

## Implementation plan
1. **Header guard + externs**
   - Define `#ifndef SENSOR_H`…`#endif`.
   - Declare the two API functions only; forbid global state in the header.
2. **Data source strategy**
   - Default input: `../data/sensor_feed.csv`.
   - Allow overrides via environment variable (e.g., `SENSOR_FEED`).
   - If the CSV is missing/empty, fall back to pseudo-random values.
3. **State management**
   - Keep static buffers/indices inside `sensor.c`.
   - Ensure `sensor_init` guards against multiple initialization calls.
4. **Testing hooks**
   - Document how to add deterministic fixtures under `tests/` for replay.

Before committing code, document any decisions or TODOs here so reviewers know what to expect once the implementation lands.
