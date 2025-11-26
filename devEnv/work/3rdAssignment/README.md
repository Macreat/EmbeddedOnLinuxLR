# Embedded Linux Service – Mock Sensor Logging Daemon (`assignment-sensor`)

This repository contains the complete implementation of a **systemd-managed background service** for the _Embedded Linux Systems Programming Assignment_.  
The service periodically samples a mock sensor (default: `/dev/urandom`) and writes **ISO-8601-timestamped log entries** to `/tmp`, with documented fallback to `/var/tmp` when required.  
The project demonstrates **modular C backend design**, **daemon lifecycle management**, **signal-safe shutdown**, and **reproducible builds** via `Makefile`.

---

## Repository Layout

| Path                   | Purpose                                                                                     |
| ---------------------- | ------------------------------------------------------------------------------------------- |
| [`src/`](src/)         | Core C implementation of the sampler (`assignment_sensor.c`).                               |
| [`systemd/`](systemd/) | Service unit file (`assignment-sensor.service`) for `systemd` integration.                  |
| [`tests/`](tests/)     | Manual and semi-automated validation procedures, including fallback and failure-mode tests. |
| [`docs/`](docs/)       | Diagrams, high-level workflow drawings, and documentation bundles.                          |
| [`docs/ai/`](docs/ai/) | Mandatory AI-development logs: `prompt-log.md`, `reflection.md`, `provenance.json`.         |
| [`build/`](build/)     | Build output directory for the generated binary (ignored by version control).               |
| `Makefile`             | Reproducible build automation, install/uninstall rules, and cleaning targets.               |
| `README.md`            | Technical documentation (this file).                                                        |

**Quick links:**

- Sampler source code → [`src/assignment_sensor.c`](src/assignment_sensor.c)
- Service definition → [`systemd/assignment-sensor.service`](systemd/assignment-sensor.service)
- Tests → [`tests/`](tests/)
- AI artifacts → [`docs/ai/`](docs/ai/)

---

## Project Workflow

### 1. Design & Planning

- Overall design and workflow derived from the embedded Linux assignment specification.
- Service requirements defined: periodic sampling, safe shutdown, fallback behavior, error handling.
- High-level architecture and module responsibilities documented under [`docs/`](docs/).

### 2. Minimal Sampler Prototype

- Implemented a baseline sampler reading a single 32-bit value from `/dev/urandom`.
- Verified compiler and environment with a minimal binary.
- Iterative expansion added:
  - CLI parameters (`--interval`, `--logfile`, `--device`)
  - ISO-8601 time formatting
  - POSIX signal-handling (`SIGTERM`, `SIGINT`)
  - nanosleep-based periodic scheduling

### 3. Daemon-Ready Backend Module (`assignment-sensor`)

- Fully line-buffered logging to avoid partial writes.
- Fallback logic: `/tmp/assignment_sensor.log` → `/var/tmp/assignment_sensor.log`.
- Immediate non-zero exit on initialization failures (device or log file).
- Safe cleanup ensuring file descriptors are closed before process termination.

### 4. Build Automation

- Makefile provides:
  - `make` / `make all` → compile into `build/assignment-sensor`
  - `make clean` → remove build artifacts
  - `make install` → deploy into `/usr/local/bin`
  - `make uninstall` → remove installed binary

### 5. Systemd Integration

- Service unit created under [`systemd/assignment-sensor.service`](systemd/assignment-sensor.service):
  - Starts at `multi-user.target` or higher.
  - Uses `Type=simple`.
  - Includes restart policy (`Restart=on-failure`, `RestartSec=2`).
  - Accepts CLI parameters directly via `ExecStart=`.

### 6. Testing & Validation

- Validation performed in both WSL/macOS container environments and native Linux.
- Manual tests document:
  - fallback behavior
  - device failure handling
  - SIGTERM shutdown
  - systemd restart behavior
  - interval-change behavior

### 7. Documentation & AI Log

- All model interactions and design refinements stored in:
  - [`docs/ai/prompt-log.md`](docs/ai/prompt-log.md)
  - [`docs/ai/reflection.md`](docs/ai/reflection.md)
  - [`docs/ai/provenance.json`](docs/ai/provenance.json)

---

## Architecture Overview

### Software Module Flow

```
assignment-sensor (main loop)
│
├──> Timestamp Formatter (ISO-8601 with ms)
│
├──> Device Reader
│ └── /dev/urandom (default)
│
└──> Log Writer
├── /tmp/assignment_sensor.log (primary)
└── /var/tmp/assignment_sensor.log (fallback)

```

### Service Lifecycle Flow

```
systemd → ExecStart=/usr/local/bin/assignment-sensor
↓
Initialisation:
- parse CLI
- open log file (fallback if needed)
- open device
- install signal handlers
↓
Steady State:
periodic sample → format → write(log)
↓
Termination:
SIGTERM/SIGINT → flush → close → exit(0)

```

---

## Build Instructions

```
cd assignment-sensor/
make # produces build/assignment-sensor
make clean # removes build directory

- binary output :

build/assignment-sensor

```

---

## Installation & Service Activation

```

sudo make install
sudo cp systemd/assignment-sensor.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now assignment-sensor.service


- verify :

systemctl status assignment-sensor.service
tail -n 10 /tmp/assignment_sensor.log
```

---

## Runtime configuration

```
 assignment-sensor [--interval <seconds>] [--logfile <path>] [--device <path>]


| Option        | Description                                                                                          |
| -------------- | ---------------------------------------------------------------------------------------------------- |
| `--interval`   | Sampling interval in seconds; must be `>0`. Default: `5`.                                            |
| `--logfile`    | Custom log path. If explicitly specified and invalid → service exits with error.                    |
| `--device`     | Path to a mock sensor device. Default: `/dev/urandom`.                                              |

To modify service defaults:

- sudo systemctl edit assignment-sensor.service
- sudo systemctl restart assignment-sensor.service


```

---

## Testing guidelines

Detailed version in [`tests/`](tests/). Summary:

1. **Standard operation**  
   Confirm periodic logs under `/tmp`.

2. **Fallback behavior**  
   Make `/tmp` non-writable; verify logs appear in `/var/tmp`.

3. **Signal handling**  
   `sudo systemctl stop assignment-sensor.service`  
   Confirm clean termination without truncated log lines.

4. **Device failure**  
   Use `--device /dev/fake0`; expect immediate failure.

5. **Automatic restart**  
   Kill process manually → `systemd` should relaunch it.

6. **Interval modification**  
   Change `--interval`; verify timing precision via log deltas.

---

## Uninstallation

```
sudo systemctl disable --now assignment-sensor.service
sudo rm /etc/systemd/system/assignment-sensor.service
sudo systemctl daemon-reload
sudo make uninstall

- optional cleanup :

rm /tmp/assignment_sensor.log
rm /var/tmp/assignment_sensor.log
```

---

## Design notes

- `/dev/urandom` selected due to non-blocking behavior appropriate for services running at boot time.
- All writes are line-buffered to ensure log integrity under asynchronous shutdown.
- The service adheres to systemd expectations:
  - exit code non-zero on startup failure
  - restartable
  - stateless between runs (logs persist by design)
- Build system is self-contained and reproducible on any standard Linux distribution.

## example Log out

```
2025-01-11T14:22:53.104Z | 0xA32F77C1
2025-01-11T14:22:58.107Z | 0x9910D4B4
2025-01-11T14:23:03.108Z | 0x12F49CDE

```

---

## References

- `systemd.service(5)`
- Linux Kernel Random API (`/dev/random`, `/dev/urandom`)
- POSIX Time Specification (`clock_gettime`, `nanosleep`)
- Assignment description and evaluation rubric (institutional document)
- Linux_Services_PSLE.pdf presentation

---

## License

Academic use only for Embedded Linux coursework.
