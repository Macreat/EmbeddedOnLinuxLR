#define _POSIX_C_SOURCE 199309L // enable POSIX time APIs

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "../sensor/sensor.h"
#include "../actuators/actuator.h"

// External actuator objects (defined in led_actuator.c and buzzer_actuator.c)
extern Actuator LED;
extern Actuator BUZZER;

// Helper: sleep in milliseconds
static void msleep(long ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

int main(void)
{
    sensor_init();           // initialize sensor module
    double threshold = 40.0; // activation threshold (light percentage)
    double value;
    struct timespec now;

    printf("Starting closed-loop controller...\n");

    while (1)
    {
        value = sensor_read(); // get sensor value
        clock_gettime(CLOCK_MONOTONIC, &now);

        printf("[%ld.%03ld] Sensor=%.2f ",
               now.tv_sec, now.tv_nsec / 1000000, value);

        if (value < threshold) // low light → activate
        {
            LED.activate();
            BUZZER.activate();
            printf("→ ACTUATORS ON\n");
        }
        else // high light → deactivate with delay
        {
            printf("→ scheduling OFF\n");
            sleep(1);
            BUZZER.deactivate(); // after 1 s
            sleep(4);
            LED.deactivate(); // +4 s = 5 s total
        }

        msleep(100); // sample every 100 ms
    }
    return 0;
}
