# AI Usage

- This design, simulation, and documentation were prepared with the assistance of **OpenAI ChatGPT (GPT-5)** and the **Codex CLI–based engineering agent** in a Linux terminal environment.

AI tools were used to **support** the engineering process, not to replace manual design or validation.  
Their role focused on accelerating structure definition, simulation setup, and documentation consistency across all modules.

---

## Scope of AI Assistance

### Repository organization

- Structured the modular directory tree:
  - `sensor/`, `actuators/`, `controller/`, `tests/`, `data/`, `docs/`, and `build/`.
- Defined initial READMEs and workflow documentation for traceability.
- Suggested naming conventions and separation between simulation and hardware layers.

### Simulation and testing

- Generated early simulation code (`test_sensor.c`, `test_actuators.c`) for CSV-based and polymorphic testing.
- Provided Makefile templates for reproducible builds and incremental testing.
- Guided integration between modules during early compilation and linking phases.

### Hardware deployment

- Suggested `pigpio`–based GPIO/SPI interface for Raspberry Pi 3 Model B.
- Produced reference wiring diagrams and code for LED, buzzer, and MCP3008.
- Verified runtime configuration of SPI and GPIO with monotonic timing.

### Documentation

- Authored per-directory READMEs and minimal reporting templates.
- Generated Markdown diagrams describing workflow and hardware/software block architecture.
- Maintained concise, standards-compliant style for technical clarity.

---

## Prompting Strategy

- Start by clarifying **module roles** (sensor, actuators, controller).
- Build **simulation scaffolds** before hardware integration.
- Iterate on **compilation and linking** steps to ensure clean reproducible builds.
- Use AI feedback to correct implicit declarations, POSIX compliance, and header guards.
- Document structure and testing artifacts directly in Markdown during the process.

---

## Example Prompts

- “Define a modular C workflow for an embedded controller with `sensor`, `actuator`, and `controller` directories and describe how to link them.”
- “Write `sensor.c` to simulate CSV input before replacing it with MCP3008 over SPI.”
- “Explain how to achieve polymorphism in C for LED and buzzer actuators using function pointers.”
- “Generate concise READMEs per directory, following a minimal technical format.”
- “Integrate Raspberry Pi GPIO and SPI hardware using pigpio and document the pin mapping.”

---

## Human Verification

All AI-generated outputs were:

- Manually reviewed, edited, and verified on real hardware.
- Adjusted for correctness, performance, and consistency.
- Integrated only after successful simulation or hardware validation.

---

## Outcome

AI assistance streamlined:

- Repository planning and code scaffolding.
- Early debugging and compilation alignment across environments.
- Documentation consistency and technical formatting.

Every final source file, test, and diagram was **authored, verified, and refined manually** before inclusion in the repository.
