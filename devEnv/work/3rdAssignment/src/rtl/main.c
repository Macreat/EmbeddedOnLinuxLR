/* main.c
 *
 * Minimal entry point.
 * The logic is fully implemented in assignment_sensor.c.
 */

#include "assignment_sensor.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    sensor_config_t cfg;

    if (!sensor_parse_cli(&cfg, argc, argv))
    {
        fprintf(stderr, "Invalid arguments.\n");
        return EXIT_FAILURE;
    }

    if (!sensor_initialize(&cfg))
    {
        fprintf(stderr, "Initialization failed.\n");
        return EXIT_FAILURE;
    }

    int rc = sensor_run_loop();
    sensor_cleanup();

    return (rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
