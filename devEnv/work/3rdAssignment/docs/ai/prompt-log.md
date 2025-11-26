# AI Prompt Log (Summary)

This document summarizes the key prompts exchanged with the AI assistant during the development of the `assignment-sensor` service.

---

## 1. Reading and interpreting the assignment

**User Prompt:**  
“Consider this assignment file. I want to implement a solid structure… break down and detail the workflow.”

**AI Response:**  
Parsed `Service assignment.md`, outlined the required service: periodic mock-sensor sampling, log fallback logic, systemd unit targeting `multi-user.target`, repository layout (`src/`, `systemd/`, `tests/`, `docs/ai/`), Makefile targets, and documentation requirements.

---

## 2. Internal planning for implementation steps

**User Prompt (internal):**  
Request for a Codex/CLI-style staged plan.

**AI Response:**  
Proposed a four-phase workflow:

1. Analyze assignment requirements
2. Implement core C program + unit file
3. Produce documentation and AI-evidence folder
4. Validate integration and produce final summary

---

## 3. Design of the C binary

**User Prompt (internal):**  
“How should the binary be structured?”

**AI Response:**  
Recommended `/dev/urandom` as a non-blocking mock device, CLI with `--interval`, `--logfile`, `--device`, structured error handling, SIGTERM/SIGINT handling using `sigaction`, line-buffered logging with fallback to `/var/tmp`, and a minimal Makefile with `install` and `uninstall`.

---

## 4. Documentation & Testing

**User Prompt (internal):**  
“Provide README structure, tests, and relevant instructions.”

**AI Response:**  
Delivered a full README outline, test procedures for fallback, error handling, signal termination, and systemd integration. Suggested separating manual tests into `tests/manual-tests.md` and documenting IA usage.

---

## 5. Modularization of the Code

**User Prompt:**  
“Transform the monolithic program into modular form using a header, a module source file, and a minimal main.”

**AI Response:**  
Generated `assignment_sensor.h`, modularized implementation (`assignment_sensor.c`), and a minimal clean `main.c` calling the public API.

---

## 6. Makefile Improvements

**User Prompt:**  
“How should the Makefile be adapted for `src/rtl` and multiple modules?”

**AI Response:**  
Provided a modern Makefile scanning all `.c` files inside `src/rtl/`, generating object files under `build/`, and adding clean install/uninstall workflows.

---

## 7. Systemd Troubleshooting

**User Prompt:**  
“Service is not producing /tmp logs.”

**AI Response:**  
Diagnosed systemd error `203/EXEC`, identified cross-architecture binary issue, recommended recompiling directly on Raspberry Pi and validating execution manually before enabling the service.

---

## 8. Manual Test Workflow

**User Prompt:**  
“Show a complete, professional manual-test flow.”

**AI Response:**  
Delivered a structured test suite: happy path, fallback behavior, SIGTERM termination, error with invalid device, restart-on-failure, interval adjustment, and full startup procedure from installation to uninstallation.

---

**Tools Used**

```json
{
  "tools": [
    {
      "name": "ChatGPT Codex (GPT-5)",
      "provider": "OpenAI",
      "date": "2025-11-20",
      "medium": "CLI / Web Interface"
    }
  ]
}
```
