/* assignment_sensor.h
 *
 * Public interface for the mock sensor logging module.
 * Provides initialization, main execution loop, and cleanup.
 */

#ifndef ASSIGNMENT_SENSOR_H
#define ASSIGNMENT_SENSOR_H

#define _POSIX_C_SOURCE 200809L

#include "assignment_sensor.h"
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
/* Struct grouping configuration parameters */
typedef struct
{
    const char *device_path; /* device to read (default: /dev/urandom) */
    const char *log_path;    /* log file path (NULL = auto default + fallback) */
    double interval_sec;     /* sampling interval */
} sensor_config_t;

/* Parse CLI arguments and fill config structure */
bool sensor_parse_cli(sensor_config_t *cfg, int argc, char *argv[]);

/* Initialize log file, device, signal handlers */
bool sensor_initialize(const sensor_config_t *cfg);

/* Execute periodic sampling loop until SIGTERM/SIGINT */
int sensor_run_loop(void);

/* Release all internal resources (device, log file) */
void sensor_cleanup(void);

#endif /* ASSIGNMENT_SENSOR_H */
