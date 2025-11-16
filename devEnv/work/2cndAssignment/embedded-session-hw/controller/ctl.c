#define _POSIX_C_SOURCE 199309L // enable POSIX time functions

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pigpio.h>

#include "../sensor/sensor.h"
#include "../actuators/actuator.h"

// External actuator objects (defined in led_actuator.c and buzzer_actuator.c)
extern Actuator LED;
extern Actuator BUZZER;

// Optional initialization prototypes from each actuator
void led_init(void);
void buzzer_init(void);

// Helper: millisecond sleep
static void msleep(long ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

int main(void)
{
    // Initialize pigpio library (required once per process)
    if (gpioInitialise() < 0)
    {
        fprintf(stderr, "Failed to initialize pigpio\n");
        return 1;
    }

    // Initialize each module
    sensor_init();
    led_init();
    buzzer_init();

    double threshold = 40.0; // activation threshold (light percentage)
    double value;
    struct timespec now;

    printf("Starting closed-loop controller (pigpio hardware version)...\n");

    while (1)
    {
        value = sensor_read(); // read LDR value (0–100%)
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

    // Cleanup (if program ever exits)
    gpioTerminate();
    return 0;
}
