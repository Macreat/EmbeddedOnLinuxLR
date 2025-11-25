/*
 * assignment_sensor.c
 *
 * Final evolution of the mock sensor sampler.
 *
 * This file shows the complete progression from the initial minimal version
 * (read one value from /dev/urandom and print it) to the final structured
 * version used by the systemd service.
 *
 * Key features:
 *  - Clean shutdown via SIGTERM/SIGINT
 *  - Command-line configuration (interval, logfile, device path)
 *  - ISO-8601 timestamps with millisecond precision
 *  - Fallback logging (/tmp → /var/tmp)
 *  - Robust error reporting
 *  - Line-buffered output to avoid partial log lines
 */

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* ------------------------------------------------------------------------- */
/* Global termination flag, modified only inside signal handler              */
/* ------------------------------------------------------------------------- */

static volatile sig_atomic_t g_should_stop = 0;

/*
 * Signal handler: set termination flag.
 * The main loop checks this flag to exit cleanly.
 */
static void handle_sigterm(int signo)
{
    (void)signo;
    g_should_stop = 1;
}

/*
 * Install handlers for SIGTERM and SIGINT.
 * Used when the process runs under systemd or manually.
 */
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
/* CLI usage                                                                 */
/* ------------------------------------------------------------------------- */

static void print_usage(const char *prog)
{
    fprintf(stderr,
            "Usage: %s [--interval <seconds>] [--logfile <path>] [--device <path>]\n"
            "Defaults: interval=5, logfile=/tmp/assignment_sensor.log (fallback /var/tmp), "
            "device=/dev/urandom\n",
            prog);
}

/* ------------------------------------------------------------------------- */
/* Logging utilities                                                         */
/* ------------------------------------------------------------------------- */

/*
 * Attempt to open a file for appending.
 * Returns true on success, false otherwise.
 */
static bool try_open_log(const char *path, FILE **out)
{
    *out = fopen(path, "a");
    if (*out)
    {
        /* line-buffering ensures atomic log entries */
        setvbuf(*out, NULL, _IOLBF, 0);
        return true;
    }
    return false;
}

/*
 * Open the primary log path (requested or default). If unavailable,
 * automatically fallback to /var/tmp, unless a custom path was explicitly given.
 */
static FILE *open_logfile(const char *requested,
                          char *resolved_path,
                          size_t resolved_size)
{
    const char *primary = requested ? requested : "/tmp/assignment_sensor.log";
    const char *fallback = "/var/tmp/assignment_sensor.log";
    FILE *fp = NULL;

    if (try_open_log(primary, &fp))
    {
        strncpy(resolved_path, primary, resolved_size - 1);
        resolved_path[resolved_size - 1] = '\0';
        return fp;
    }

    /* If user explicitly provided a path, do not fallback silently */
    if (requested)
    {
        fprintf(stderr, "Failed to open log file '%s': %s\n",
                primary, strerror(errno));
        return NULL;
    }

    /* Automatic fallback */
    if (try_open_log(fallback, &fp))
    {
        strncpy(resolved_path, fallback, resolved_size - 1);
        resolved_path[resolved_size - 1] = '\0';
        fprintf(stderr,
                "Warning: '/tmp' not writable, using fallback '%s'\n",
                fallback);
        return fp;
    }

    fprintf(stderr,
            "Failed to open both '%s' and '%s': %s\n",
            primary, fallback, strerror(errno));
    return NULL;
}

/* ------------------------------------------------------------------------- */
/* Device access                                                             */
/* ------------------------------------------------------------------------- */

/*
 * Open a mock sensor device (default: /dev/urandom).
 */
static FILE *open_device(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        fprintf(stderr,
                "Failed to open device '%s': %s\n",
                path, strerror(errno));
        return NULL;
    }
    return fp;
}

/* ------------------------------------------------------------------------- */
/* Timestamp formatting                                                      */
/* ------------------------------------------------------------------------- */

/*
 * Produce an ISO-8601 timestamp in UTC (YYYY-MM-DDTHH:MM:SS.mmmZ).
 * Returns true on success.
 */
static bool format_timestamp(char *buffer,
                             size_t size,
                             long *millis_out)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return false;

    struct tm tm_utc;
    if (!gmtime_r(&ts.tv_sec, &tm_utc))
        return false;

    if (strftime(buffer, size, "%Y-%m-%dT%H:%M:%S", &tm_utc) == 0)
        return false;

    *millis_out = ts.tv_nsec / 1000000L;
    return true;
}

/* ------------------------------------------------------------------------- */
/* Sampling                                                                  */
/* ------------------------------------------------------------------------- */

/*
 * Read one 32-bit random value and write a formatted log line.
 * Returns 0 on success, negative on failure.
 */
static int sample_and_log(FILE *device, FILE *logf)
{
    unsigned int value = 0;
    size_t read = fread(&value, 1, sizeof(value), device);
    if (read != sizeof(value))
    {
        fprintf(stderr, "Short read from device\n");
        return -1;
    }

    char tsbuf[32];
    long millis = 0;

    if (!format_timestamp(tsbuf, sizeof(tsbuf), &millis))
    {
        fprintf(stderr, "Failed to format timestamp\n");
        return -1;
    }

    if (fprintf(logf, "%s.%03ldZ | 0x%08X\n",
                tsbuf, millis, value) < 0)
    {
        fprintf(stderr, "Failed to write log line\n");
        return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* CLI parsing                                                               */
/* ------------------------------------------------------------------------- */

/*
 * Parse a double with full validation.
 */
static double parse_double(const char *s)
{
    char *end = NULL;
    double v = strtod(s, &end);

    if (!end || end == s || *end != '\0')
        return -1.0;

    return v;
}

/* ------------------------------------------------------------------------- */
/* Main program                                                              */
/* ------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    const char *device_path = "/dev/urandom";
    const char *log_path = NULL;
    double interval = 5.0;

    /* ---- Parse CLI arguments ---- */
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--interval") == 0 && i + 1 < argc)
        {
            interval = parse_double(argv[++i]);
            if (interval <= 0.0)
            {
                fprintf(stderr,
                        "Invalid interval: must be > 0\n");
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(argv[i], "--logfile") == 0 && i + 1 < argc)
        {
            log_path = argv[++i];
        }
        else if (strcmp(argv[i], "--device") == 0 && i + 1 < argc)
        {
            device_path = argv[++i];
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
        else
        {
            fprintf(stderr,
                    "Unknown or incomplete option: %s\n",
                    argv[i]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    /* ---- Open log file ---- */
    char resolved_log[256] = {0};
    FILE *logf = open_logfile(log_path,
                              resolved_log,
                              sizeof(resolved_log));

    if (!logf)
        return EXIT_FAILURE;

    /* ---- Open device ---- */
    FILE *device = open_device(device_path);
    if (!device)
    {
        fclose(logf);
        return EXIT_FAILURE;
    }

    /* ---- Install signal handlers ---- */
    install_signal_handlers();

    /* ---- Main periodic sampling loop ---- */
    while (!g_should_stop)
    {
        if (sample_and_log(device, logf) != 0)
        {
            fclose(device);
            fclose(logf);
            return EXIT_FAILURE;
        }

        /* Sleep for "interval" seconds */
        struct timespec req;
        req.tv_sec = (time_t)interval;
        req.tv_nsec = (long)((interval - req.tv_sec) * 1e9);

        /* Restart sleep if interrupted by signal */
        while (nanosleep(&req, &req) == -1 && errno == EINTR)
        {
            if (g_should_stop)
                break;
        }
    }

    /* ---- Clean shutdown ---- */
    fclose(device);
    fclose(logf);

    return EXIT_SUCCESS;
}
