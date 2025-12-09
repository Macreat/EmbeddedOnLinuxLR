#include "actuator.h"
#include <pigpio.h>
#include <stdio.h> // <-- ADD THIS

#define BUZZ_PIN 27

void buzzer_init(void)
{
    gpioSetMode(BUZZ_PIN, PI_OUTPUT);
}

static void buzzer_on(void) { gpioWrite(BUZZ_PIN, 1); }
static void buzzer_off(void) { gpioWrite(BUZZ_PIN, 0); }
static int buzzer_status(void) { return gpioRead(BUZZ_PIN); }

Actuator BUZZER = {
    .activate = buzzer_on,
    .deactivate = buzzer_off,
    .status = buzzer_status};