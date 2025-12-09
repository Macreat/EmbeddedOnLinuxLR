# Controller Blueprint

`ctl.c` will live here. It ties the sensor and actuator layers together into a closed-loop system per the homework brief.

## Requirements recap

- Poll the sensor every 100 ms using monotonic time (`clock_gettime(CLOCK_MONOTONIC)`).
- Threshold crossings:
  - `>= threshold`: activate LED and buzzer immediately; cancel pending off timers.
  - `< threshold`: schedule buzzer off after 1 s and LED off after 5 s.
- Log each iteration with timestamp, sensor value, and actuator states.

## Suggested workflow

1. **Configuration parsing**
   - Support optional CLI args `[threshold] [iterations]`.
2. **Timer helpers**
   - Implement small structs/functions for scheduling/canceling off timers to keep `main` readable.
3. **Main loop**
   - Acquire monotonic timestamp.
   - Read sensor, update actuators, handle timers, emit log line.
4. **Build targets**
   - Integrate with root `Makefile` (`ctl64`, `ctl32`) once the code exists.
5. **Logging artifacts**
   - Store representative run outputs under `../tests/` for documentation.

Add design notes or TODOs here before coding to keep reviewers aligned.
