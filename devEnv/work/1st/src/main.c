/**
 * @file sysmon_tui_full.c
 * @brief Embedded Linux Resource Monitor (Enhanced TUI Version)
 *
 * Displays CPU and memory usage dynamically using ncurses.
 * Includes per-core CPU load, total memory, and usage percentage.
 *
 * Author: Mateo Almeida (Macreat)
 * Course: Embedded Systems on Linux – UNAL 2025-2
 */
// includes and definitions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <ncurses.h>
// constants and macros
#define PROC_CPUINFO "/proc/cpuinfo"
#define PROC_MEMINFO "/proc/meminfo"
#define PROC_STAT "/proc/stat"
#define REFRESH_INTERVAL 2
// data structures
typedef struct
{
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CpuTimes;

// Utility functions

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
// Read CPU info from /proc/cpuinfo
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
// Read memory info from /proc/meminfo
void read_meminfo(unsigned long long *total, unsigned long long *avail)
{
    FILE *f = fopen(PROC_MEMINFO, "r");
    if (!f)
        return;
    char line[128];
    while (fgets(line, sizeof(line), f))
    {
        sscanf(line, "MemTotal: %llu kB", total);
        sscanf(line, "MemAvailable: %llu kB", avail);
    }
    fclose(f);
}
// Read CPU times from /proc/stat
int read_cpu_times(CpuTimes **times_out, size_t *count_out)
{
    FILE *f = fopen(PROC_STAT, "r");
    if (!f)
        return -1;
    CpuTimes *times = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char line[512];

    while (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "cpu", 3) != 0)
            break;
        if (!isdigit((unsigned char)line[3]))
            continue; // skip aggregate line
        if (count == capacity)
        {
            size_t newcap = capacity == 0 ? 4 : capacity * 2;
            CpuTimes *tmp = realloc(times, newcap * sizeof(CpuTimes));
            if (!tmp)
            {
                free(times);
                fclose(f);
                return -1;
            }
            times = tmp;
            capacity = newcap;
        }
        CpuTimes c = {0};
        sscanf(line, "cpu%*u %llu %llu %llu %llu %llu %llu %llu %llu",
               &c.user, &c.nice, &c.system, &c.idle,
               &c.iowait, &c.irq, &c.softirq, &c.steal);
        times[count++] = c;
    }
    fclose(f);
    *times_out = times;
    *count_out = count;
    return 0;
}

// Compute CPU loads given previous and current times

void compute_cpu_loads(const CpuTimes *prev, const CpuTimes *curr, size_t count, double *loads)
{
    for (size_t i = 0; i < count; ++i)
    {
        unsigned long long prev_idle = prev[i].idle + prev[i].iowait;
        unsigned long long idle = curr[i].idle + curr[i].iowait;
        unsigned long long prev_non_idle = prev[i].user + prev[i].nice + prev[i].system +
                                           prev[i].irq + prev[i].softirq + prev[i].steal;
        unsigned long long non_idle = curr[i].user + curr[i].nice + curr[i].system +
                                      curr[i].irq + curr[i].softirq + curr[i].steal;
        unsigned long long prev_total = prev_idle + prev_non_idle;
        unsigned long long total = idle + non_idle;
        unsigned long long total_diff = total - prev_total;
        unsigned long long idle_diff = idle - prev_idle;
        loads[i] = total_diff ? (double)(total_diff - idle_diff) * 100.0 / total_diff : 0.0;
    }
}

// main program

int main(void)
{
    char cpu_model[128];
    int cores = 0;
    unsigned long long mem_total, mem_free;

    CpuTimes *prev_times = NULL;
    size_t prev_count = 0;

    // ncurses init
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);

    while (1)
    {
        read_cpuinfo(cpu_model, sizeof(cpu_model), &cores);
        read_meminfo(&mem_total, &mem_free);

        CpuTimes *curr_times = NULL;
        size_t curr_count = 0;
        if (read_cpu_times(&curr_times, &curr_count) != 0)
            break;

        double *loads = NULL;
        int have_loads = 0;
        if (prev_times && prev_count == curr_count)
        {
            loads = malloc(curr_count * sizeof(double));
            if (loads)
            {
                compute_cpu_loads(prev_times, curr_times, curr_count, loads);
                have_loads = 1;
            }
        }

        double mem_used_pct = 100.0 * (1.0 - ((double)mem_free / mem_total));
        char total_str[32], free_str[32];
        format_kib(mem_total, total_str, sizeof(total_str));
        format_kib(mem_free, free_str, sizeof(free_str));

        erase();

        attron(COLOR_PAIR(2));
        mvprintw(0, 0, "=======================================================");
        mvprintw(1, 0, " Embedded Linux Resource Monitor (TUI version)");
        mvprintw(2, 0, "=======================================================");
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(3));
        mvprintw(4, 0, "Processor        : %s", cpu_model[0] ? cpu_model : "Unknown");
        mvprintw(5, 0, "Logical cores    : %d", cores);
        attroff(COLOR_PAIR(3));

        attron(COLOR_PAIR(1));
        mvprintw(7, 0, "Memory total     : %s", total_str);
        mvprintw(8, 0, "Memory available : %s", free_str);
        mvprintw(9, 0, "Memory used      : %.2f%%", mem_used_pct);
        attroff(COLOR_PAIR(1));

        mvprintw(11, 0, "CPU load per core:");
        if (have_loads)
        {
            for (size_t i = 0; i < curr_count; ++i)
                mvprintw(12 + i, 2, "cpu%-2zu -> %6.2f%%", i, loads[i]);
        }
        else
        {
            mvprintw(12, 2, "Gathering data...");
        }

        attron(COLOR_PAIR(4));
        mvprintw(13 + curr_count, 0, "\nRefresh interval : %d seconds", REFRESH_INTERVAL);
        mvprintw(15 + curr_count, 0, "[Press 'q' to quit]");
        attroff(COLOR_PAIR(4));

        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q')
            break;

        free(prev_times);
        free(loads);
        prev_times = curr_times;
        prev_count = curr_count;

        sleep(REFRESH_INTERVAL);
    }

    endwin();
    free(prev_times);
    return 0;
}
