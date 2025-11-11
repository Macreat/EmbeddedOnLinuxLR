#define _POSIX_C_SOURCE 200809L
#include "sensor.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SENSOR_MAX_VALUES 1024
#define SENSOR_DEFAULT_FEED "data/sensor_feed.csv"

static double sensor_values[SENSOR_MAX_VALUES];
static size_t sensor_value_count;
static size_t sensor_index;
static bool sensor_uses_random;
static bool sensor_initialized;

static void seed_random_once(void) {
    static bool seeded = false;
    if (!seeded) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        srand((unsigned)(ts.tv_nsec ^ ts.tv_sec));
        seeded = true;
    }
}

static void load_sensor_feed(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr,
                "sensor: unable to open feed '%s': %s – falling back to random\n",
                path, strerror(errno));
        sensor_uses_random = true;
        return;
    }

    double value = 0.0;
    while (sensor_value_count < SENSOR_MAX_VALUES &&
           fscanf(fp, "%lf", &value) == 1) {
        sensor_values[sensor_value_count++] = value;
    }
    fclose(fp);

    if (sensor_value_count == 0) {
        fprintf(stderr,
                "sensor: feed '%s' contained no samples – falling back to random\n",
                path);
        sensor_uses_random = true;
    } else {
        sensor_uses_random = false;
    }
}

void sensor_init(void) {
    if (sensor_initialized) {
        return;
    }

    const char *feed_path = getenv("SENSOR_FEED");
    if (!feed_path || feed_path[0] == '\0') {
        feed_path = SENSOR_DEFAULT_FEED;
    }

    load_sensor_feed(feed_path);
    if (sensor_uses_random) {
        seed_random_once();
    }

    sensor_initialized = true;
}

double sensor_read(void) {
    if (!sensor_initialized) {
        sensor_init();
    }

    if (sensor_uses_random || sensor_value_count == 0) {
        return (double)(rand() % 10001) / 100.0;
    }

    double sample = sensor_values[sensor_index];
    sensor_index = (sensor_index + 1) % sensor_value_count;
    return sample;
}
