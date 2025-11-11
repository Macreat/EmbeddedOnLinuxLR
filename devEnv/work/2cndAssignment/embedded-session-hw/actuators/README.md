# Actuator Interface Plan

This folder will host the polymorphic actuator interface plus the LED and buzzer backends required by the assignment.

## Files to create
- `actuator.h` – declares `struct actuator`, a `void *params` payload, and `activate/deactivate/status` function pointers.
- `led_actuator.c` – implements the interface for GPIO/LED behavior.
- `buzzer_actuator.c` – implements the interface for buzzer/PWM behavior.

## Workflow
1. **Design the struct**
   - Provide typedefs for the function pointers.
   - Keep the struct self-contained so callers only interact through the API; no controller-specific state leaks in.
2. **Backend allocation**
   - Each backend should expose `*_actuator_create/destroy` helpers that allocate/free any private state.
   - Document expected labels/pins in this README once hardware mappings are chosen.
3. **Polymorphic usage**
   - Ensure both implementations fill the same function pointer slots so the controller can call `act->activate(act)` generically.
4. **Testing hooks**
   - Outline how to stub the functions for unit tests (e.g., fake params structs recorded under `tests/`).

Capture any open questions (e.g., hardware abstraction decisions) below before writing code.
