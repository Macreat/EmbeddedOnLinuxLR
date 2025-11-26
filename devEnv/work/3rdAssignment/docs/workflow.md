# assignment workflow: Systemd Service that Logs a Mock Sensor

## 1. Project Purpose

Implement a Linux service that runs as a daemon, starts automatically at the `multi-user.target` boot stage, and periodically records readings from a simulated sensor.  
The project emulates a common component in embedded Linux systems: automated background tasks, continuous monitoring, and robust long-running processes.

## 2. Learning Objectives

- Package a compiled program as a systemd service.
- Understand the full lifecycle of a Linux service.
- Integrate a daemon with the filesystem, `/dev`, system signals, and boot targets.
- Document a reproducible workflow: clone, build, install, test, uninstall.
- Track and reflect on AI-assisted development.

## 3. Repository Structure

Directory used to archive AI prompt evolution and reference material required to deploy the service (like sources, binaries, docs, etc).

```

├── src/ # source program code
├── systemd/ using .service as unity
├── tests/ # notes or scripts of proof
├── docs/ # ai use and reference documentation
├── Makefile # construction system for automatization
└── README.md # general documentation


```

## 4. Program Design

- Executable written in a compiled language (C recommended).
- Configurable parameters:
  - `--interval` (seconds)
  - `--logfile` (path)
  - `--device` (defaults to `/dev/urandom`)
- Line format for logging:

```
ISO8601_TIMESTAMP | HEX_VALUE
```

--
Automatic fallback for logs: write to `/tmp` first, then `/var/tmp`.

- Clean shutdown via proper handling of `SIGTERM`.

## 5. Sensor Source

- Simulated sensor: `/dev/urandom`.
- Rationale: non-blocking entropy source available on embedded Linux.
- Important difference: `/dev/random` MAY BLOCK when system entropy is low.

## 6. systemd Unit

Suggested file: `systemd/assignment-sensor.service`

The unit should include:

- Service type: `simple`
- Execution of the binary with default parameters
- Restart policy: `Restart=on-failure`
- Installation target: `WantedBy=multi-user.target`

## 7. Recommended Workflow

1. Create the initial repository structure.
2. Implement device reading and output formatting.
3. Add a CLI parser for configurable parameters.
4. Implement signal handling for clean shutdown.
5. Write the Makefile with build, clean, and install targets.
6. Create the systemd unit and test it locally.
7. Document test cases in `tests/`.
8. Register prompts and reasoning steps in `docs/ai/`.
9. Review, clean, and prepare the repository for submission.

## 8. Tests to Perform

- Confirm service starts at `multi-user.target`.
- Verify correct logging into `/tmp`.
- Validate fallback behavior when `/tmp` is not writable.
- Confirm clean shutdown without partial log lines.
- Ensure proper failure behavior with invalid device input.
- Optional test: automatic restart when the process is terminated unexpectedly.

## 9. AI-Assisted Development Documentation

The `docs/ai/` directory must include:

- `prompt-log.md`
- `reflection.md` (maximum 500 words)
- `provenance.json`

It should record design decisions, reasoning, and manual validation for each AI-assisted iteration.

## 10. Expected Deliverables

- Git repository containing the full required structure.
- Clear, reproducible README.
- Fully functional systemd unit.
- Source code compilable on any standard Linux system.
- Documented evidence of AI usage.
