# Embedded Systems Programming – Homework Assignment

## Context and Goals

In this assignment, you will practice essential concepts of the compilation and linking process in C, the use of headers and `extern`, and the design of wrappers/interfaces. You will gradually build a **closed-loop controller** that samples a sensor and drives actuators (LED and buzzer). The project will be developed in three stages:

1. Modular **sensor library**
2. Polymorphic **actuator interface** with two backends
3. A **controller** that integrates both to implement threshold-based closed-loop behavior

You are allowed (and encouraged) to use an AI assistant. You must **document your interaction process** as part of the deliverables.

---

## Exercises

### Exercise 1 — Sensor Library

- Define `sensor.h` and `sensor.c`.
- Provide:
  - `void sensor_init(void);`
  - `double sensor_read(void);`
- The sensor may return random values or replay values from a CSV file.
- Ensure header guards and correct use of `extern`.

Problem (no guards; def inside `math.h`):

```objectivec
    main.c
    ├── #include "geometry.h"
    │       └── #include "math.h"   <-- 1st inclusion of math.h
    └── #include "math.h"           <-- 2nd inclusion of math.h
                 ^
                 └─ contains a definition → redefinition error
```

Fix (guards; decl in .h, def in .c):

```objectivec
   main.c
    ├── #include "geometry.h"
    │       └── #include "math.h"   (guarded)
    └── #include "math.h"           (guarded)

   math.h   → declarations only (+ include guard)
   math.c   → single definition; linked once
```

### Exercise 2 — Actuator Interface

- Define `actuator.h` with:
  - `void *params;`
  - Function pointers: `activate`, `deactivate`, `status`.
- Implement:
  - `led_actuator.c`
  - `buzzer_actuator.c`
- Demonstrate polymorphism by handling both through the same interface.

### Exercise 3 — Closed-Loop Controller

- Implement `ctl.c` that:
  - Samples the sensor every 100 ms.
  - If value ≥ threshold: activate LED & buzzer immediately; cancel deactivation timers.
  - If value < threshold: schedule buzzer off after 1 s, LED off after 5 s.
- Use **monotonic time**.
- Log timestamp, sensor value, LED and buzzer states.
- Build 64-bit and 32-bit binaries; inspect with `file` and `readelf`.

---

## Requirements

- Separate headers and sources with include guards.
- One definition per global symbol; use `extern` for declarations.
- Provide a `Makefile` with targets:
  - `make ctl64`
  - `make ctl32` (this part may require you to install 32 bit cross compiling tools)
  - `make clean`
- Code must compile with `gcc -Wall -Wextra -std=c11`.
- Document AI usage:
  - File `ai_log.md` containing prompts and answers.
  - Add short comments on why each prompt was asked and how you refined them.

---

## Suggested Repository Skeleton

```ASCII
embedded-session-hw/
├── Makefile
├── README.md
├── sensor/
│ ├── sensor.h
│ └── sensor.c
├── actuators/
│ ├── actuator.h
│ ├── led_actuator.c
│ └── buzzer_actuator.c
├── controller/
│ └── ctl.c
└── tests/
└── sensor_feed.csv
```

---

## Deliverables

- **Source code** in the repo structure.
- **Makefile** for 64-bit and 32-bit builds.
- **README.md** with:
  - Build instructions
  - Execution logs
  - Binary inspection (`file`, `readelf`)
  - Reflection on compile vs link errors, wrappers
- **AI interaction log** (`ai_log.md`)

---

## Evaluation Criteria

- Correct implementation & behavior — **30%**
- Code organization & headers/extern usage — **15%**
- Build reproducibility — **10%**
- Documentation clarity — **10%**
- Reflection on concepts — **10%**
- **AI interaction log — 25%**
  - Completeness of prompts/answers — 10%
  - Prompt quality — 10%
  - Coherent evolution of prompts — 5%
