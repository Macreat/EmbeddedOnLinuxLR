#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SYSFS_GPIO_DIR "/sys/class/gpio"

static int mq_gpio = 23;       /* GPIO BCM del pin digital del MQ  2*/
static int led_gpio = 24;      /* GPIO BCM del LED */
static bool led_active_low;    /* invertir si el LED enciende con nivel bajo */
static const char *edge_mode = "both";

static int mq_fd = -1;
static int led_fd = -1;
static volatile sig_atomic_t keep_running = 1;

static void on_signal(int sig)
{
    (void)sig;
    keep_running = 0;
}

static int write_str(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    ssize_t written = write(fd, value, strlen(value));
    close(fd);
    return (written == (ssize_t)strlen(value)) ? 0 : -1;
}

static int export_gpio(int gpio)
{
    char path[64];
    snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d", gpio);
    if (access(path, F_OK) == 0) {
        return 0; /* ya exportado */
    }
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", gpio);
    return write_str(SYSFS_GPIO_DIR "/export", buf);
}

static int unexport_gpio(int gpio)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", gpio);
    return write_str(SYSFS_GPIO_DIR "/unexport", buf);
}

static int set_direction(int gpio, const char *dir)
{
    char path[64];
    snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
    return write_str(path, dir);
}

static int set_edge(int gpio, const char *edge)
{
    char path[64];
    snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);
    return write_str(path, edge);
}

static int open_value_fd(int gpio, int flags)
{
    char path[64];
    snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
    return open(path, flags);
}

static int read_gpio_value(int fd)
{
    char buf[4] = {0};
    if (lseek(fd, 0, SEEK_SET) < 0) {
        return -1;
    }
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n <= 0) {
        return -1;
    }
    return (buf[0] == '0') ? 0 : 1;
}

static int write_led_value(int fd, int level)
{
    char val = level ? '1' : '0';
    if (lseek(fd, 0, SEEK_SET) < 0) {
        return -1;
    }
    return (write(fd, &val, 1) == 1) ? 0 : -1;
}

static void cleanup(void)
{
    if (led_fd >= 0) {
        close(led_fd);
        led_fd = -1;
    }
    if (mq_fd >= 0) {
        close(mq_fd);
        mq_fd = -1;
    }
    unexport_gpio(led_gpio);
    unexport_gpio(mq_gpio);
}

static void print_usage(const char *prog)
{
    fprintf(stderr,
            "Uso: %s [--mq-pin N] [--led-pin N] [--led-active-low] [--edge both|rising|falling]\n",
            prog);
    fprintf(stderr, "Por defecto: mq-pin=23, led-pin=24, edge=both, LED activo en alto.\n");
}

static int parse_args(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--mq-pin") == 0 && i + 1 < argc) {
            mq_gpio = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--led-pin") == 0 && i + 1 < argc) {
            led_gpio = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--led-active-low") == 0) {
            led_active_low = true;
        } else if (strcmp(argv[i], "--edge") == 0 && i + 1 < argc) {
            edge_mode = argv[++i];
        } else {
            print_usage(argv[0]);
            return -1;
        }
    }
    return 0;
}

static int init_gpio(void)
{
    if (export_gpio(mq_gpio) < 0 || export_gpio(led_gpio) < 0) {
        perror("No se pudo exportar GPIO");
        return -1;
    }

    /* pequenas pausas para permitir que el kernel cree los nodos */
    usleep(100000);

    if (set_direction(mq_gpio, "in") < 0) {
        perror("No se pudo poner MQ como entrada");
        return -1;
    }

    if (set_edge(mq_gpio, edge_mode) < 0) {
        perror("No se pudo configurar edge para MQ");
        return -1;
    }

    if (set_direction(led_gpio, "out") < 0) {
        perror("No se pudo poner LED como salida");
        return -1;
    }

    mq_fd = open_value_fd(mq_gpio, O_RDONLY | O_NONBLOCK);
    if (mq_fd < 0) {
        perror("No se pudo abrir value del MQ");
        return -1;
    }

    led_fd = open_value_fd(led_gpio, O_WRONLY);
    if (led_fd < 0) {
        perror("No se pudo abrir value del LED");
        return -1;
    }

    /* estado inicial: LED segun nivel del sensor */
    int mq_level = read_gpio_value(mq_fd);
    if (mq_level < 0) {
        mq_level = 0;
    }
    int led_level = led_active_low ? !mq_level : mq_level;
    if (write_led_value(led_fd, led_level) < 0) {
        perror("No se pudo fijar estado inicial del LED");
        return -1;
    }

    return 0;
}

static void event_loop(void)
{
    struct pollfd pfd = {
        .fd = mq_fd,
        .events = POLLPRI | POLLERR,
        .revents = 0,
    };

    /* limpiar primer valor antes del poll */
    read_gpio_value(mq_fd);

    while (keep_running) {
        int ret = poll(&pfd, 1, 1000);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll");
            break;
        }
        if (ret == 0) {
            continue; /* timeout para permitir Ctrl+C */
        }
        if (pfd.revents & (POLLPRI | POLLERR)) {
            int mq_level = read_gpio_value(mq_fd);
            if (mq_level < 0) {
                perror("read mq");
                continue;
            }
            int led_level = led_active_low ? !mq_level : mq_level;
            if (write_led_value(led_fd, led_level) < 0) {
                perror("write led");
            }
            printf("MQ nivel %d -> LED %s\n",
                   mq_level,
                   (led_level ? "ON" : "OFF"));
            fflush(stdout);
        }
    }
}

int main(int argc, char **argv)
{
    if (parse_args(argc, argv) < 0) {
        return EXIT_FAILURE;
    }

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);
    atexit(cleanup);

    if (init_gpio() < 0) {
        cleanup();
        return EXIT_FAILURE;
    }

    printf("Escuchando flancos %s en GPIO%d (MQ). LED en GPIO%d (%s).\n",
           edge_mode, mq_gpio, led_gpio,
           led_active_low ? "activo en bajo" : "activo en alto");
    fflush(stdout);

    event_loop();

    printf("\nSaliendo y liberando GPIO...\n");
    return EXIT_SUCCESS;
}
