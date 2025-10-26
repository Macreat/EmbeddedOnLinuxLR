/**
 * @file sysmon_simple.c
 * @brief Simplified Embedded Linux Resource Monitor for /proc data
 *
 * This program reads system information directly from Linux virtual files:
 *   - /proc/cpuinfo   → processor name and logical cores
 *   - /proc/meminfo   → memory usage
 *   - /proc/stat      → CPU load per core
 *
 * It prints the data in a simple TUI-like layout (without ncurses),
 * updating every 2 seconds.
 *
 * Author: Mateo Almeida (Macreat)
 * Course: Embedded Systems on Linux – UNAL 2025-2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROC_CPUINFO "/proc/cpuinfo"
#define PROC_MEMINFO "/proc/meminfo"
#define PROC_STAT "/proc/stat"
#define REFRESH_INTERVAL 2

// ----------------------------
//  Utility functions
// ----------------------------

/**
 * @brief Converts kilobytes to human-readable format (MiB or GiB).
 */
void format_kib(unsigned long long kib, char *out, size_t size)
{
    const char *units[] = {"KiB", "MiB", "GiB"};
    double val = (double)kib;
    int i = 0;
    while (val >= 1024.0 && i < 2)
    {
        val /= 1024.0;
        i++;
    }
    snprintf(out, size, "%.2f %s", val, units[i]);
}

/**
 * @brief Extracts CPU model and number of cores.
 */
void read_cpuinfo(char *model, size_t size, int *cores)
{
    FILE *f = fopen(PROC_CPUINFO, "r");
    if (!f)
        return;

    *cores = 0;
    model[0] = '\0';
    char line[256];

    while (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "model name", 10) == 0 && model[0] == '\0')
        {
            char *colon = strchr(line, ':');
            if (colon)
                snprintf(model, size, "%s", colon + 2);
            model[strcspn(model, "\n")] = '\0';
        }
        else if (strncmp(line, "processor", 9) == 0)
            (*cores)++;
    }
    fclose(f);
}

/**
 * @brief Extracts total and available memory from /proc/meminfo.
 */
void read_meminfo(unsigned long long *total, unsigned long long *free)
{
    FILE *f = fopen(PROC_MEMINFO, "r");
    if (!f)
        return;
    char line[128];
    while (fgets(line, sizeof(line), f))
    {
        sscanf(line, "MemTotal: %llu kB", total);
        sscanf(line, "MemAvailable: %llu kB", free);
    }
    fclose(f);
}

/**
 * @brief Extracts aggregated CPU utilization from /proc/stat.
 * @return percentage usage of total CPU (not per core).
 */
double read_cpu_usage()
{
    static unsigned long long prev_total = 0, prev_idle = 0;

    FILE *f = fopen(PROC_STAT, "r");
    if (!f)
        return 0.0;

    char label[8];
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    fscanf(f, "%s %llu %llu %llu %llu %llu %llu %llu %llu",
           label, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    fclose(f);

    unsigned long long idle_time = idle + iowait;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;
    unsigned long long total = idle_time + non_idle;

    unsigned long long total_diff = total - prev_total;
    unsigned long long idle_diff = idle_time - prev_idle;

    prev_total = total;
    prev_idle = idle_time;

    if (total_diff == 0)
        return 0.0;
    return (double)(total_diff - idle_diff) * 100.0 / (double)total_diff;
}

// ----------------------------
//  Main monitor
// ----------------------------

int main(void)
{
    char cpu_model[128];
    int cores = 0;
    unsigned long long mem_total, mem_free;
    double cpu_usage = 0.0;

    while (1)
    {
        // Leer datos del sistema
        read_cpuinfo(cpu_model, sizeof(cpu_model), &cores);
        read_meminfo(&mem_total, &mem_free);
        cpu_usage = read_cpu_usage();

        // Calcular uso de memoria
        double used_pct = 100.0 * (1.0 - ((double)mem_free / mem_total));
        char total_str[32], free_str[32];
        format_kib(mem_total, total_str, sizeof(total_str));
        format_kib(mem_free, free_str, sizeof(free_str));

        // Limpiar pantalla
        printf("\033[2J\033[H");

        // Mostrar información
        printf("==============================================\n");
        printf(" Embedded Linux Resource Monitor (Simple)\n");
        printf("==============================================\n\n");

        printf("Processor        : %s", cpu_model[0] ? cpu_model : "Unknown\n");
        printf("\nLogical cores    : %d\n", cores);
        printf("CPU usage (avg)  : %.2f%%\n\n", cpu_usage);

        printf("Memory total     : %s\n", total_str);
        printf("Memory available : %s\n", free_str);
        printf("Memory used      : %.2f%%\n", used_pct);

        printf("\nRefreshing every %d seconds... (Ctrl+C to exit)\n", REFRESH_INTERVAL);

        fflush(stdout);
        sleep(REFRESH_INTERVAL);
    }
    return 0;
}
