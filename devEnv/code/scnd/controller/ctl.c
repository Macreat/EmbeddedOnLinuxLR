#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../actuators/actuator.h"
#include "../sensor/sensor.h"

#define SAMPLE_PERIOD_MS 100L
#define BUZZER_OFF_DELAY_MS 1000L
#define LED_OFF_DELAY_MS 5000L

typedef struct {
    double threshold;
    size_t iterations;
} controller_config_t;

typedef struct {
    bool active;
    struct timespec deadline;
} off_timer_t;

static double now_to_seconds(const struct timespec *ts) {
    return (double)ts->tv_sec + (double)ts->tv_nsec / 1e9;
}

static void add_ms(struct timespec *ts, long delta_ms) {
    ts->tv_sec += delta_ms / 1000L;
    ts->tv_nsec += (delta_ms % 1000L) * 1000000L;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec += 1L;
        ts->tv_nsec -= 1000000000L;
    }
}

static void schedule_timer(off_timer_t *timer, const struct timespec *base,
                           long delay_ms) {
    timer->deadline = *base;
    add_ms(&timer->deadline, delay_ms);
    timer->active = true;
}

static void cancel_timer(off_timer_t *timer) {
    timer->active = false;
}

static bool timer_expired(const off_timer_t *timer, const struct timespec *now) {
    if (!timer->active) {
        return false;
    }
    if (now->tv_sec > timer->deadline.tv_sec) {
        return true;
    }
    if (now->tv_sec == timer->deadline.tv_sec &&
        now->tv_nsec >= timer->deadline.tv_nsec) {
        return true;
    }
    return false;
}

static void usage(const char *prog) {
    fprintf(stderr,
            "Usage: %s [threshold] [iterations]\n"
            "  threshold  – trigger level (default 65.0)\n"
            "  iterations – sample count (default 200)\n",
            prog);
}

static controller_config_t parse_args(int argc, char **argv) {
    controller_config_t cfg = {
        .threshold = 65.0,
        .iterations = 200,
    };

    if (argc > 1) {
        char *end = NULL;
        cfg.threshold = strtod(argv[1], &end);
        if (end == argv[1]) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (argc > 2) {
        char *end = NULL;
        long iter = strtol(argv[2], &end, 10);
        if (end == argv[2] || iter <= 0) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        cfg.iterations = (size_t)iter;
    }
    return cfg;
}

static void sleep_ms(long ms) {
    struct timespec req = {
        .tv_sec = ms / 1000L,
        .tv_nsec = (ms % 1000L) * 1000000L,
    };
    while (nanosleep(&req, &req) == -1 && errno == EINTR) {
        continue;
    }
}

int main(int argc, char **argv) {
    controller_config_t cfg = parse_args(argc, argv);

    sensor_init();

    actuator_t led = led_actuator_create("status-led");
    actuator_t buzzer = buzzer_actuator_create("alert-buzzer");
    if (!led.activate || !buzzer.activate) {
        fprintf(stderr, "Failed to initialize actuators\n");
        led_actuator_destroy(&led);
        buzzer_actuator_destroy(&buzzer);
        return EXIT_FAILURE;
    }

    off_timer_t led_timer = {0};
    off_timer_t buzzer_timer = {0};

    for (size_t i = 0; i < cfg.iterations; ++i) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        double sample = sensor_read();

        if (sample >= cfg.threshold) {
            led.activate(&led);
            buzzer.activate(&buzzer);
            cancel_timer(&led_timer);
            cancel_timer(&buzzer_timer);
        } else {
            if (!led_timer.active) {
                schedule_timer(&led_timer, &now, LED_OFF_DELAY_MS);
            }
            if (!buzzer_timer.active) {
                schedule_timer(&buzzer_timer, &now, BUZZER_OFF_DELAY_MS);
            }
        }

        if (timer_expired(&led_timer, &now)) {
            led.deactivate(&led);
            cancel_timer(&led_timer);
        }
        if (timer_expired(&buzzer_timer, &now)) {
            buzzer.deactivate(&buzzer);
            cancel_timer(&buzzer_timer);
        }

        double timestamp = now_to_seconds(&now);
        printf("[%.6f] sensor=%.2f led=%s buzzer=%s\n", timestamp, sample,
               led.status(&led) ? "ON" : "OFF",
               buzzer.status(&buzzer) ? "ON" : "OFF");
        fflush(stdout);

        sleep_ms(SAMPLE_PERIOD_MS);
    }

    led_actuator_destroy(&led);
    buzzer_actuator_destroy(&buzzer);
    return EXIT_SUCCESS;
}
