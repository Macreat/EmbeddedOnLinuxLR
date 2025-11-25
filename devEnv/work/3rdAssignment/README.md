# Assignment Workflow: Mock Sensor Logging Service (systemd)

This project implements a production-grade, systemd-managed background service responsible for periodically sampling a mock sensor (default `/dev/urandom`) and writing time-stamped records to a log file under `/tmp` with an automatic fallback to `/var/tmp` when required.  
The goal is to practice embedded Linux backend-service design, reproducible builds, clean shutdown behavior, and integration with the Linux init system.

---

## 1. Overview

The service is implemented as a compiled C program (`assignment-sensor`) and packaged as a `systemd` unit that starts at the `multi-user.target` stage.  
The sampler supports configurable interval, device path, logging path, proper signal handling (SIGTERM/SIGINT), and robust error reporting.  
The service is designed for use in embedded Linux systems with constrained runtime environments and strict reliability requirements.

---

## 2. Project Structure

The repository follows a modular layout suitable for embedded Linux coursework and scalable system design.

```


├── src/ # Program source code
│ └── assignment_sensor.c
├── systemd/ # Systemd unit definition
│ └── assignment-sensor.service
├── tests/ # Manual/automated test procedures
│ └── README.md
├── docs/
│ └── ai/ # Required AI development documentation
│ ├── prompt-log.md
│ ├── reflection.md
│ └── provenance.json
├── Makefile # Build, install, uninstall targets
└── README.md # Main documentation (this file)


```

### Directory links

- [`src/`](src/) — implementation of the sensor sampler
- [`systemd/`](systemd/) — service unit file
- [`tests/`](tests/) — validation procedures
- [`docs`](docs/) — docs and AI-assisted development documentation

---

## 3. Requirements

- Linux system with `systemd` (tested with systemd 245+)
- Standard development toolchain:
  - `make`
  - C compiler (`gcc` or `clang`)
- Superuser permissions for:
  - installing binaries under `/usr/local/bin`
  - installing service files into `/etc/systemd/system`
  - starting/reloading systemd services

---

## 4. Environment Setup

```

sudo apt update
sudo apt install git build-essential

- clone the repository and enter the project


git clone <repository-url>
cd <repository>

```

---

## 5. Build System

The build is controlled via the top-level `Makefile`.

Targets:

- `make` / `make all` — build into `build/assignment-sensor`
- `make clean` — remove build artifacts
- `make install` — install binary into `/usr/local/bin/`
- `make uninstall` — remove installed binary

Example:

```

make
make clean

- output binary path :

build/assignment-sensor
```

---

## 6. Program Description

### Features

- Compiled C binary
- Configurable at runtime using CLI parameters
- ISO-8601 timestamps with millisecond precision
- Graceful termination on `SIGTERM` and `SIGINT`
- Log file fallback logic (`/tmp` → `/var/tmp`)
- Non-blocking mock sensor input via `/dev/urandom`
- Line-buffered output to prevent partial records

### CLI Interface

```

assignment-sensor [--interval <seconds>] [--logfile <path>] [--device <path>]

```

Defaults:

- Interval: `5` seconds
- Log path: `/tmp/assignment_sensor.log` (fallback `/var/tmp/assignment_sensor.log`)
- Device: `/dev/urandom`

Notes:

- If a custom `--logfile` is explicitly set and cannot be opened, the program exits with an error.
- If no `--logfile` is provided and `/tmp` is not writable, fallback is automatically chosen and reported on `stderr`.

---

## 7. Systemd Integration

The service unit is located at [`systemd/assignment-sensor.service`](systemd/assignment-sensor.service).

### Unit Behavior

- Starts at `multi-user.target`
- Uses `Type=simple`
- Restarts on failure (`Restart=on-failure`)
- Default interval set via ExecStart arguments
- Logs stored under `/tmp` or `/var/tmp`

### Installation

```
sudo make install
sudo cp systemd/assignment-sensor.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now assignment-sensor.service

```

### Status & Logs

```
systemctl status assignment-sensor.service
tail -n 20 /tmp/assignment_sensor.log

```

---

## 8. Testing Procedures

All manual test procedures are documented in [`tests/`](tests/).

### Summary of Key Tests

1. **Happy path**:  
   Verify periodic log generation under `/tmp`.

2. **Fallback behavior**:  
   Run in environment where `/tmp` is not writable → logs must appear in `/var/tmp`.

3. **Graceful shutdown**:  
   Stopping the service must terminate cleanly without truncated lines.

4. **Device error handling**:  
   Using an invalid device path must cause immediate failure with non-zero exit code.

5. **Automatic restart (optional)**:  
   Killing the process must trigger restart by systemd.

---

## 9. AI Development Documentation

As required by the assignment, all AI usage is documented under:

- [`docs/ai/prompt-log.md`](docs/ai/prompt-log.md)
- [`docs/ai/reflection.md`](docs/ai/reflection.md)
- [`docs/ai/provenance.json`](docs/ai/provenance.json)

The documentation includes:

- Iterative prompts used during development
- Summaries of AI contributions
- Validation and corrections performed manually
- Model and tool provenance data

---

## 10. Uninstallation

```
sudo systemctl disable --now assignment-sensor.service
sudo rm /etc/systemd/system/assignment-sensor.service
sudo systemctl daemon-reload
sudo make uninstall

- optional cleanup


rm /tmp/assignment_sensor.log
rm /var/tmp/assignment_sensor.log


```

---

## 11. Design Notes

- The project uses `/dev/urandom` because it is non-blocking and universally available on Linux systems.
- Logging is line-buffered to avoid partial writes and maintain integrity under abrupt termination.
- Initialization failures produce immediate non-zero exit codes, ensuring systemd detects malfunctions.
- The repository is structured for educational use in backend-service development for embedded Linux environments.

---

## 12. License

This project is provided for academic and instructional use within embedded Linux coursework.
