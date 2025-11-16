# Testing & Verification Notes

This folder is used to capture evidence that the controller and modules behave correctly under SIMULATION before deployment on hardware.

## Included test programs

| File               | Purpose                                                                                        |
| ------------------ | ---------------------------------------------------------------------------------------------- |
| `test_sensor.c`    | Reads 10 simulated sensor values using `sensor_read()` (from CSV or random source).            |
| `test_actuators.c` | Toggles LED and buzzer through the shared `Actuator` interface to verify polymorphic behavior. |

### Example commands

```bash
# Compile and run sensor test
gcc -Wall -Wextra -std=c11 sensor/sensor.c tests/test_sensor.c -o tests/test_sensor
./tests/test_sensor

# Compile and run actuator test
gcc -Wall -Wextra -std=c11 actuators/led_actuator.c actuators/buzzer_actuator.c tests/test_actuators.c -o tests/test_actuators
./tests/test_actuators
```

## notes

-Keep raw outputs here to document reproducible runs.

-Tests validate that sensor readings, actuator activations, and linking between modules work before full system integration.
