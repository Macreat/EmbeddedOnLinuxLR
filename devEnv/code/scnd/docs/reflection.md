# Reflection

- **Compile vs. link discipline:** Headers (`sensor.h`, `actuator.h`) only expose declarations and typedefs. All definitions remain in `.c` files, preventing multiple-definition link errors even when modules include each other through nested headers.
- **Wrapper/interfaces:** `actuator_t` hides backend state in `void *params` with function pointers, mirroring how GPIO/PWM drivers are abstracted on Linux-based embedded systems. The controller never needs to know whether it is toggling an LED or buzzer.
- **Timing considerations:** All delays/timers use `CLOCK_MONOTONIC`, so adjustments to wall-clock time (e.g., via NTP or `date`) do not skew actuator scheduling.
- **32-bit build challenges:** Building `ctl32` may require multilib packages (`gcc-multilib` on Debian/Ubuntu). Document any missing headers/errors in `build_notes.md` when setting up on WSL or Raspberry Pi.
