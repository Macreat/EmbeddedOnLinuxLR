#include <stdio.h>
#include "../actuators/actuator.h"

// external references for each actuator object

extern Actuator LED;
extern Actuator BUZZER;

// main function to test actuators

int main(void)
{
       printf("Testing actuators...");

       LED.activate();
       BUZZER.activate();

       printf("LED status: %d | BUZZER state: %d\n",
              LED.status(), BUZZER.status());

       LED.deactivate();
       BUZZER.deactivate();

       printf("LED state: %d | BUZZER state: %d\n",
              LED.status(), BUZZER.status());

       return 0;
}