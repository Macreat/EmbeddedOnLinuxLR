# Actuators Module

This directory defines and implements the **actuator interface layer** of the closed-loop controller.  
It demonstrates _polymorphism in C_ through the use of **function pointers inside a struct**, allowing different actuator backends (LED, buzzer, etc.) to share a unified API.

---

## Polymorphism in C

C does not have classes, but we can achieve _polymorphism_ by embedding **function pointers** inside a struct.

## Hardware mapping

```


| Actuator | GPIO (BCM) | Pin | Description      |
| -------- | ---------- | --- | ---------------- |
| LED      | 17         | 11  | Visual indicator |
| Buzzer   | 27         | 13  | Audible alert    |

Both are configured as outputs in their respective \*\_init() functions.

```

## Usage

These files are compiled and linked with the controller:

```
gcc -Wall -Wextra -std=c11 controller/ctl.c \
actuators/led_actuator.c actuators/buzzer_actuator.c ...

```

## Notes

-Keep actuator logic self-contained: GPIO setup and cleanup should be done locally.

-Adding a new actuator (e.g., fan, relay) only requires writing a new .c file and instantiating a new Actuator object — the controller code remains unchanged.
