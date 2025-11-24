# Embedded Linux — Assignment: Systemd Service that Logs a Mock Sensor

## Brief
Build a `systemd` service that starts at **`multi-user.target` (or higher)** and periodically samples a **mock sensor** (e.g., `/dev/random`) and appends **timestamp + value** to a debug log under `/tmp` when available (fallback allowed; document your choice).  

The sampler must be written in a **compiled language** (C/C++/Rust/Go, etc.).  
**Deliverable:** a public or private **Git repository** with code, unit file, and documentation (clone, build, install, uninstall).  
**Use of AI assistants is required and assessed.**

---

## Learning Objectives
- Package a compiled program as a `systemd` service.  
- Handle logging, configuration, termination signals, and basic reliability.  
- Practice **effective AI-assisted development** (prompt engineering, iteration, verification).  
- Produce reproducible builds and deployment instructions.

---

## Functional Requirements
1. **Compiled program** (C/C++/Rust/Go/…):
   - Reads a mock sensor at a **configurable interval** (CLI flag or config file; sensible default).  
   - Logs **ISO-8601 timestamp** + **sampled value** (human-readable) to a file under `/tmp` if writable; otherwise use a documented fallback path (e.g., `/var/tmp`).  
   - Runs continuously until stopped; exits non-zero on fatal init errors; handles **SIGTERM** gracefully (flush/close).  
2. **Systemd unit**: starts at or above `multi-user.target`, enable/disable with `systemctl`.  
3. **Documentation**: clear **clone → build → install → enable → test → disable → uninstall** steps.

> You may choose any safe mock device or generator (e.g., `/dev/urandom`, a small PRNG, a synthetic counter). Justify the choice briefly in the README.

---

## Non-Functional Requirements
- One-command build (`make`, `cargo`, `go build`, CMake, etc.) and a clean target.  
- Reasonable logging format, line-buffered or equivalent to avoid partial lines.  
- No interpreted runtimes for the main sampler; helper scripts allowed only for tests/build tooling.  
- If running as non-root, document file permissions and any `User=` settings in the unit.

---

## Evidence of AI-Assisted Development (Required)
Create an `ai/` folder in the repo containing:
- **`prompt-log.md`**: chronological prompts and condensed AI responses you used to design, code, debug, or document.  
- **`reflection.md`** (≤500 words): what you asked, why you iterated, what you accepted/rejected, how you validated outputs, and what you changed manually.  
- **`provenance.json`**: tool/model names & versions (e.g., “ChatGPT GPT-5, 2025-09-19”), plus dates.  

> Summarize long AI responses and link to commits instead of pasting code blobs.  

**Academic integrity:** You may use AI to ideate, sketch code, or explain concepts; **you remain responsible** for correctness, security, and licensing. Cite any external snippets with their license.

---

## Repository Structure (Suggested)

```
.
├─ src/ # source code
├─ systemd/ # your .service unit(s)
│ └─ assignment-sensor.service
├─ tests/ # scripts or notes for manual tests
├─ ai/ # prompt-log.md, reflection.md, provenance.json
├─ Makefile / Cargo.toml / go.mod / CMakeLists.txt
└─ README.md # clone/build/install/uninstall/test docs
``` 

---

## Documentation Checklist (README.md)

### Clone & Build
- Prereqs (compiler, `systemd` version if relevant).  
- Commands (e.g., `git clone … && cd … && make`).  
- Produced artifact path (e.g., `./build/assignment-sensor`).  

### Install & Enable
- Copy binary path (e.g., `/usr/local/bin/assignment-sensor`).  
- Copy unit: `/etc/systemd/system/assignment-sensor.service`.  
- `sudo systemctl daemon-reload`  
- `sudo systemctl enable --now assignment-sensor.service`  

### Configuration
- CLI flags or config file location/format (interval, logfile path, device path).  
- Default values and examples.  

### Testing
- Verify running status (`systemctl status`), log path, example log lines.  
- Fallback behavior demo (what happens if `/tmp` isn’t writable).  
- Graceful shutdown test (`systemctl stop`).  

### Uninstall
- `systemctl disable --now …`  
- Remove unit & binary; `systemctl daemon-reload`.  

---

## Evaluation Rubric (100%)

**A. Documentation & Repro (30%)**  
- Clear, exact **clone/build/install/uninstall** steps; config and defaults documented.  
- Explanation of `/tmp` fallback and any permissions/user settings.  

**B. Correctness & Reliability (30%)**  
- Service starts at `multi-user.target` or higher; logs timestamp+value at the configured interval; handles SIGTERM; non-zero exit on fatal errors.  
- No partial lines; reasonable error handling; no busy-looping.  

**C. Test Design & Evidence (20%)**  
- Tests (automated or scripted manual steps) that cover: start/stop, normal logging, fallback path, restart behavior (`Restart=` optional but documented), and error handling.  
- Expected outcomes included (sample logs, `journalctl` snippets).  

**D. AI Usage Quality (20%)**  
- **Prompt strategy:** specificity, constraints, iterative refinement.  
- **Critical judgment:** identifies and fixes AI mistakes; cites what was modified by hand.  
- **Traceability:** prompt log links to commits; provenance recorded.  

---

## Minimal Guidance (No Full Code)

### Required CLI Interface (example — you may design your own)

```
assignment-sensor [--interval <seconds>] [--logfile <path>] [--device <path>]
``` 

### Runtime Behaviors
- On start: parse CLI/config, open log file (fallback if needed), open device.  
- Loop: read one sample, get current time, append `ISO8601_TIMESTAMP | VALUE`.  
- On SIGTERM: stop loop, flush/close, return 0.  
- On fatal init errors: print to stderr and **return non-zero**.  

### Systemd Unit (skeletal — adapt to your design)
```ini
[Unit]
Description=Mock sensor logger (assignment)
After=multi-user.target

[Service]
Type=simple
ExecStart=/usr/local/bin/assignment-sensor --interval 5 --logfile /tmp/assignment_sensor.log
Restart=on-failure
RestartSec=2

[Install]
WantedBy=multi-user.target
```
---

## Suggested Tests

1. **Happy Path**  
   - Start the service.  
   - After 2–3 sampling intervals, run:  
     ```bash
     tail /tmp/assignment_sensor.log
     ```  
   - You should see lines like:  
     ```
     2025-09-19T15:03:21.123Z | 0xDEADBEEF
     ```

2. **Fallback Path**  
   - Run the service in an environment where `/tmp` is not writable (e.g., container, chroot, or remount).  
   - Verify that the service logs to the documented fallback path (e.g., `/var/tmp/assignment_sensor.log`).  

3. **SIGTERM Handling**  
   - Stop the service with:  
     ```bash
     sudo systemctl stop assignment-sensor.service
     ```  
   - Confirm the service exits cleanly and that no partial log lines are written.  

4. **Failure Path**  
   - Start the service with a non-existent device (e.g., `--device /dev/fake0`).  
   - The service should fail fast, print an error, and return a **non-zero exit code**.  

5. **(Optional) Restart Test**  
   - Configure the unit with `Restart=on-failure`.  
   - Manually kill the process to simulate a crash.  
   - Confirm systemd automatically restarts the service.  

---

## Submission

- Provide a **Git repository URL** (with read access for the instructor).  
- Ensure the following files exist under the `ai/` folder:  
  - `prompt-log.md`  
  - `reflection.md`  
  - `provenance.json`  
- The repository must build cleanly on a **fresh machine or SBC** by following the instructions in your `README.md`.  
- Include clear documentation on:  
  - Cloning the repo  
  - Building the program  
  - Installing and enabling the service  
  - Testing normal, fallback, and failure behavior  
  - Disabling and uninstalling the service  

---
