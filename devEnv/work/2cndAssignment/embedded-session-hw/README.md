# Embedded Session HW – Implementation Blueprint

This directory only contains the **organizational skeleton** for the homework defined in `devEnv/work/2cndAssignment/docs/Compilation linking interfaces homework.md`. No source code has been committed yet; the goal is to outline how to fulfill the assignment on Linux servers, WSL, or Raspberry Pi 3 Model B targets.

## Directory map

| Path          | Purpose                                                                                             |
| ------------- | --------------------------------------------------------------------------------------------------- |
| `sensor/`     | Specs + workflow for creating `sensor.h/.c` with guarded declarations and a CSV/random backend.     |
| `actuators/`  | Interface notes for `actuator.h` and the LED/Buzzer implementations that share one polymorphic API. |
| `controller/` | Runbook for designing `ctl.c`, timers, and logging loop.                                            |
| `tests/`      | Guidance on runtime validation, log capture, and binary inspection artifacts.                       |
| `data/`       | Placeholder for `sensor_feed.csv` or other input traces.                                            |
| `docs/`       | Documentation deliverables (README, AI log, reflections, build notes).                              |
| `build/`      | Output bin/obj artifacts once compilation steps are executed (kept empty until builds run).         |

## Workflow overview

1. **Planning** – Re-read the assignment Markdown, copy any acceptance criteria or checklists into `docs/README.md`, and sketch test cases before touching code.
2. **Sensor module** – Follow `sensor/README.md` to define guarded headers, a deterministic CSV replay path (`data/sensor_feed.csv`), and a random fallback. Ensure no globals are defined in headers.
3. **Actuator interface** – Use `actuators/README.md` to design the polymorphic struct with function pointers and backend-specific cleanup helpers.
4. **Controller** – Implement sampling, timers, and logging per `controller/README.md`, keeping time API usage monotonic and isolating policy in unit-testable helpers.
5. **Build tooling** – Introduce a `Makefile` (not yet present) that emits `build/ctl64` and `build/ctl32` using `gcc -Wall -Wextra -std=c11`, plus `make clean`. Document any multilib setup in `docs/README.md`.
6. **Testing & inspection** – Capture representative runs and tool outputs (`file`, `readelf`) under `tests/`, cross-referenced from the project README.
7. **AI log** – Record each assistant interaction in `docs/ai_log.md`, including rationale and refinements, before marking the homework complete.

> **Reminder:** Do not start coding until the plan, inputs, and validation steps are agreed upon. This skeleton is meant to keep the deliverables tidy and traceable.
