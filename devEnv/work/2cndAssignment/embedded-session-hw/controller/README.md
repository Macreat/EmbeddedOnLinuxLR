# Controller Blueprint

`ctl.c` lives here. It ties the sensor and actuator layers into a closed-loop system per the homework brief.

## Requirements recap

- Poll the sensor every 100 ms using monotonic time (`clock_gettime(CLOCK_MONOTONIC)`).
- Threshold crossings:
  - `>= threshold`: activate LED and buzzer immediately; cancel pending off timers.
  - `< threshold`: schedule buzzer off after 1 s and LED off after 5 s.
- Log each iteration with timestamp, sensor value, and actuator states.

## workflow

1. Configuration parsing – optional CLI args `[threshold] [iterations]`.
2. Timer helpers – structs/functions for scheduling off timers.
3. Main loop – sample, compare, update actuators, and log.
4. Build targets – integrate with root `Makefile` (`ctl64`, `ctl32`).
5. Logging – store representative runs under `../tests/`.
