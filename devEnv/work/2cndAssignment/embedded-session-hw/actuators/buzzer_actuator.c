#include "actuator.h"
#include <stdio.h>

static int buzzerState = 0;

static void buzzerOn(void)
{
    buzzerState = 1;
    printf("Buzzer ON\n");
}
static void buzzerOff(void)
{
    buzzerState = 0;
    printf("Buzzer OFF\n");
}
static int buzzerStatus(void) { return buzzerState; }

// define an instance of Actuator for Buzzer

Actuator BUZZER = {
    .activate = buzzerOn,
    .deactivate = buzzerOff,
    .status = buzzerStatus};