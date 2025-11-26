/* assignment_sensor.c
 *
 * Modular implementation of the mock sensor sampler.
 * Contains all internal logic: CLI parsing, logging, device access,
 * timestamp formatting, signal handling, and the main execution loop.
 */

#include "assignment_sensor.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* ------------------------------------------------------------------------- */
/* Internal static state (hidden from main.c)                                */
/* ------------------------------------------------------------------------- */

static volatile sig_atomic_t g_should_stop = 0;
static FILE *g_logf = NULL;
static FILE *g_device = NULL;

/* ------------------------------------------------------------------------- */
/* Signal handling                                                           */
/* ------------------------------------------------------------------------- */

static void handle_sigterm(int signo)
{
    (void)signo;
    g_should_stop = 1;
}

static void install_signal_handlers(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
}

/* ------------------------------------------------------------------------- */
/* Logging                                                                   */
/* ------------------------------------------------------------------------- */

static bool try_open_log(const char *path, FILE **out)
{
    *out = fopen(path, "a");
    if (*out)
    {
        setvbuf(*out, NULL, _IOLBF, 0);
        return true;
    }
    return false;
}

static FILE *open_logfile(const char *requested, char *resolved, size_t sz)
{
    const char *primary = requested ? requested : "/tmp/assignment_sensor.log";
    const char *fallback = "/var/tmp/assignment_sensor.log";

    FILE *fp = NULL;

    if (try_open_log(primary, &fp))
    {
        strncpy(resolved, primary, sz - 1);
        resolved[sz - 1] = '\0';
        return fp;
    }

    /* If user explicitly requested a file, do not fallback */
    if (requested)
    {
        fprintf(stderr, "Failed to open '%s': %s\n", primary, strerror(errno));
        return NULL;
    }

    /* Automatic fallback */
    if (try_open_log(fallback, &fp))
    {
        strncpy(resolved, fallback, sz - 1);
        resolved[sz - 1] = '\0';
        fprintf(stderr, "Warning: using fallback log '%s'\n", fallback);
        return fp;
    }

    fprintf(stderr, "Failed to open both log paths\n");
    return NULL;
}

/* ------------------------------------------------------------------------- */
/* Device access                                                             */
/* ------------------------------------------------------------------------- */

static FILE *open_device(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        fprintf(stderr, "Failed to open device '%s': %s\n", path, strerror(errno));
    }
    return fp;
}

/* ------------------------------------------------------------------------- */
/* Timestamps and sampling                                                   */
/* ------------------------------------------------------------------------- */

static bool format_timestamp(char *buf, size_t sz, long *ms)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return false;

    struct tm tm_utc;
    if (!gmtime_r(&ts.tv_sec, &tm_utc))
        return false;

    if (strftime(buf, sz, "%Y-%m-%dT%H:%M:%S", &tm_utc) == 0)
        return false;

    *ms = ts.tv_nsec / 1000000L;
    return true;
}

static int sample_once(void)
{
    unsigned int value = 0;

    if (fread(&value, 1, sizeof(value), g_device) != sizeof(value))
    {
        fprintf(stderr, "Short read from device\n");
        return -1;
    }

    char ts[32];
    long ms = 0;

    if (!format_timestamp(ts, sizeof(ts), &ms))
    {
        fprintf(stderr, "Timestamp formatting failed\n");
        return -1;
    }

    if (fprintf(g_logf, "%s.%03ldZ | 0x%08X\n", ts, ms, value) < 0)
    {
        fprintf(stderr, "Log write failure\n");
        return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Public API                                                                */
/* ------------------------------------------------------------------------- */

bool sensor_parse_cli(sensor_config_t *cfg, int argc, char *argv[])
{
    cfg->device_path = "/dev/urandom";
    cfg->log_path = NULL;
    cfg->interval_sec = 5.0;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--interval") == 0 && i + 1 < argc)
        {
            double v = strtod(argv[++i], NULL);
            if (v <= 0.0)
                return false;
            cfg->interval_sec = v;
        }
        else if (strcmp(argv[i], "--logfile") == 0 && i + 1 < argc)
        {
            cfg->log_path = argv[++i];
        }
        else if (strcmp(argv[i], "--device") == 0 && i + 1 < argc)
        {
            cfg->device_path = argv[++i];
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool sensor_initialize(const sensor_config_t *cfg)
{
    char resolved[256];

    g_logf = open_logfile(cfg->log_path, resolved, sizeof(resolved));
    if (!g_logf)
        return false;

    g_device = open_device(cfg->device_path);
    if (!g_device)
        return false;

    install_signal_handlers();
    return true;
}

int sensor_run_loop(void)
{
    const double interval = 0.0 + ((sensor_config_t *)0)->interval_sec; /* replaced later */

    /* The real interval comes from the config stored outside → no global here */
    /* We pass it through nanosleep each iteration */

    while (!g_should_stop)
    {
        if (sample_once() != 0)
            return -1;

        struct timespec req;
        req.tv_sec = (time_t)interval;
        req.tv_nsec = (long)((interval - req.tv_sec) * 1e9);

        while (nanosleep(&req, &req) == -1 && errno == EINTR)
            if (g_should_stop)
                break;
    }

    return 0;
}

void sensor_cleanup(void)
{
    if (g_device)
        fclose(g_device);
    if (g_logf)
        fclose(g_logf);
}
