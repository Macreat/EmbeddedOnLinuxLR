#include "actuator.h"
#include <stdio.h>

static int ledState = 0; // define internal LED state

static void lenOn(void)
{
    ledState = 1;
    printf("[LED] ON\n");
}
static void lenOff(void)
{
    ledState = 0;
    printf("[LED] OFF\n");
}
static int ledStatus(void) { return ledState; }

// define a public instance of the actuator interface

Actuator LED = {
    .activate = lenOn,
    .deactivate = lenOff,
    .status = ledStatus};