#include "actuator.h"
#include <pigpio.h>
#include <stdio.h> // <-- ADD THIS

#define LED_PIN 17

void led_init(void)
{
    if (gpioInitialise() < 0)
        perror("pigpio init failed");
    gpioSetMode(LED_PIN, PI_OUTPUT);
}

static void led_on(void) { gpioWrite(LED_PIN, 1); }
static void led_off(void) { gpioWrite(LED_PIN, 0); }
static int led_status(void) { return gpioRead(LED_PIN); }

Actuator LED = {
    .activate = led_on,
    .deactivate = led_off,
    .status = led_status};