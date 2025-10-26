#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROC_STAT "/proc/stat"
#define PROC_CPUINFO "/proc/cpuinfo"
#define PROC_MEMINFO "/proc/meminfo"
#define REFRESH_INTERVAL_SECONDS 2

typedef struct
{
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
} CpuTimes;

typedef struct
{
    char model_name[128];
    int core_count;
} CpuInfo;

typedef struct
{
    unsigned long long mem_total_kb;
    unsigned long long mem_available_kb;
    unsigned long long mem_free_kb;
    unsigned long long buffers_kb;
    unsigned long long cached_kb;
    unsigned long long swap_total_kb;
    unsigned long long swap_free_kb;
    int has_mem_available;
} MemoryInfo;

static void trim_newline(char *str)
{
    if (!str)
    {
        return;
    }
    size_t len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
    {
        str[len - 1] = '\0';
    }
}

static void format_kib(unsigned long long kib, char *buffer, size_t buffer_size)
{
    static const char *suffixes[] = {"KiB", "MiB", "GiB", "TiB"};
    double value = (double)kib;
    size_t suffix_index = 0;

    while (value >= 1024.0 && suffix_index < (sizeof(suffixes) / sizeof(suffixes[0])) - 1)
    {
        value /= 1024.0;
        ++suffix_index;
    }

    snprintf(buffer, buffer_size, "%.2f %s", value, suffixes[suffix_index]);
}

static int read_cpuinfo(CpuInfo *info)
{
    FILE *fp = fopen(PROC_CPUINFO, "r");
    if (!fp)
    {
        return -1;
    }

    info->model_name[0] = '\0';
    info->core_count = 0;

    char line[512];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strncmp(line, "model name", 10) == 0)
        {
            char *colon = strchr(line, ':');
            if (colon != NULL)
            {
                colon++;
                while (*colon == ' ' || *colon == '\t')
                {
                    colon++;
                }
                strncpy(info->model_name, colon, sizeof(info->model_name) - 1);
                info->model_name[sizeof(info->model_name) - 1] = '\0';
                trim_newline(info->model_name);
            }
        }
        else if (strncmp(line, "processor", 9) == 0)
        {
            info->core_count++;
        }
    }

    fclose(fp);
    return 0;
}

static int read_meminfo(MemoryInfo *info)
{
    FILE *fp = fopen(PROC_MEMINFO, "r");
    if (!fp)
    {
        return -1;
    }

    memset(info, 0, sizeof(*info));
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "MemTotal: %llu kB", &info->mem_total_kb) == 1)
        {
            continue;
        }
        if (sscanf(line, "MemAvailable: %llu kB", &info->mem_available_kb) == 1)
        {
            info->has_mem_available = 1;
            continue;
        }
        if (sscanf(line, "MemFree: %llu kB", &info->mem_free_kb) == 1)
        {
            continue;
        }
        if (sscanf(line, "Buffers: %llu kB", &info->buffers_kb) == 1)
        {
            continue;
        }
        if (sscanf(line, "Cached: %llu kB", &info->cached_kb) == 1)
        {
            continue;
        }
        if (sscanf(line, "SwapTotal: %llu kB", &info->swap_total_kb) == 1)
        {
            continue;
        }
        if (sscanf(line, "SwapFree: %llu kB", &info->swap_free_kb) == 1)
        {
            continue;
        }
    }

    fclose(fp);
    return 0;
}

static int read_cpu_times(CpuTimes **out_times, size_t *out_count)
{
    FILE *fp = fopen(PROC_STAT, "r");
    if (!fp)
    {
        return -1;
    }

    CpuTimes *times = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char line[512];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strncmp(line, "cpu", 3) != 0)
        {
            break;
        }
        if (!isdigit((unsigned char)line[3]))
        {
            continue; /* skip aggregated line */
        }

        if (count == capacity)
        {
            size_t new_capacity = capacity == 0 ? 4 : capacity * 2;
            CpuTimes *tmp = realloc(times, new_capacity * sizeof(CpuTimes));
            if (!tmp)
            {
                free(times);
                fclose(fp);
                return -1;
            }
            times = tmp;
            capacity = new_capacity;
        }

        CpuTimes current = {0};
        unsigned long long guest = 0;
        unsigned long long guest_nice = 0;
        int parsed = sscanf(
            line,
            "cpu%*u %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
            &current.user,
            &current.nice,
            &current.system,
            &current.idle,
            &current.iowait,
            &current.irq,
            &current.softirq,
            &current.steal,
            &guest,
            &guest_nice);

        if (parsed < 4)
        {
            continue;
        }

        times[count++] = current;
    }

    fclose(fp);

    if (count == 0)
    {
        free(times);
        errno = EINVAL;
        return -1;
    }

    *out_times = times;
    *out_count = count;
    return 0;
}

static void compute_cpu_loads(const CpuTimes *prev, const CpuTimes *curr, size_t count, double *loads)
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

        unsigned long long total_diff = 0;
        if (total >= prev_total)
        {
            total_diff = total - prev_total;
        }

        unsigned long long idle_diff = 0;
        if (idle >= prev_idle)
        {
            idle_diff = idle - prev_idle;
        }

        if (total_diff == 0)
        {
            loads[i] = 0.0;
        }
        else
        {
            loads[i] = (double)(total_diff - idle_diff) * 100.0 / (double)total_diff;
        }
    }
}

static unsigned long long calculate_physical_used(const MemoryInfo *info)
{
    if (info->has_mem_available && info->mem_total_kb >= info->mem_available_kb)
    {
        return info->mem_total_kb - info->mem_available_kb;
    }

    if (info->mem_total_kb >= info->mem_free_kb + info->buffers_kb + info->cached_kb)
    {
        return info->mem_total_kb - info->mem_free_kb - info->buffers_kb - info->cached_kb;
    }

    if (info->mem_total_kb >= info->mem_free_kb)
    {
        return info->mem_total_kb - info->mem_free_kb;
    }

    return 0;
}

static unsigned long long calculate_virtual_used(const MemoryInfo *info)
{
    if (info->swap_total_kb >= info->swap_free_kb)
    {
        return info->swap_total_kb - info->swap_free_kb;
    }
    return 0;
}

static void print_error(const char *label)
{
    if (errno != 0)
    {
        fprintf(stderr, "Failed to read %s: %s\n", label, strerror(errno));
    }
    else
    {
        fprintf(stderr, "Failed to read %s\n", label);
    }
}

int main(void)
{
    CpuTimes *previous_times = NULL;
    size_t previous_count = 0;
    int exit_code = EXIT_SUCCESS;

    while (1)
    {
        CpuInfo cpu_info;
        MemoryInfo mem_info;
        CpuTimes *current_times = NULL;
        size_t current_count = 0;

        if (read_cpuinfo(&cpu_info) != 0)
        {
            print_error(PROC_CPUINFO);
            exit_code = EXIT_FAILURE;
            break;
        }
        if (read_meminfo(&mem_info) != 0)
        {
            print_error(PROC_MEMINFO);
            exit_code = EXIT_FAILURE;
            break;
        }
        if (read_cpu_times(&current_times, &current_count) != 0)
        {
            print_error(PROC_STAT);
            exit_code = EXIT_FAILURE;
            break;
        }

        double *loads = NULL;
        int have_loads = 0;
        if (previous_times != NULL && previous_count == current_count)
        {
            loads = malloc(current_count * sizeof(double));
            if (loads)
            {
                compute_cpu_loads(previous_times, current_times, current_count, loads);
                have_loads = 1;
            }
        }

        unsigned long long physical_used_kb = calculate_physical_used(&mem_info);
        unsigned long long virtual_used_kb = calculate_virtual_used(&mem_info);

        char installed_buffer[32];
        char physical_used_buffer[32];
        char virtual_used_buffer[32];
        char physical_used_pct_buffer[32];
        char virtual_used_pct_buffer[32];

        format_kib(mem_info.mem_total_kb, installed_buffer, sizeof(installed_buffer));
        format_kib(physical_used_kb, physical_used_buffer, sizeof(physical_used_buffer));
        format_kib(virtual_used_kb, virtual_used_buffer, sizeof(virtual_used_buffer));

        double physical_pct = 0.0;
        if (mem_info.mem_total_kb > 0)
        {
            physical_pct = (double)physical_used_kb * 100.0 / (double)mem_info.mem_total_kb;
        }
        double virtual_pct = 0.0;
        if (mem_info.swap_total_kb > 0)
        {
            virtual_pct = (double)virtual_used_kb * 100.0 / (double)mem_info.swap_total_kb;
        }

        snprintf(physical_used_pct_buffer, sizeof(physical_used_pct_buffer), "%.2f%%", physical_pct);
        if (mem_info.swap_total_kb > 0)
        {
            snprintf(virtual_used_pct_buffer, sizeof(virtual_used_pct_buffer), "%.2f%%", virtual_pct);
        }
        else
        {
            snprintf(virtual_used_pct_buffer, sizeof(virtual_used_pct_buffer), "n/a");
        }

        printf("\033[2J\033[H");
        printf("Embedded Linux Resource Monitor\n");
        printf("--------------------------------------\n\n");

        printf("Processor          : %s\n", cpu_info.model_name[0] ? cpu_info.model_name : "Unknown");
        printf("Logical cores      : %d\n", cpu_info.core_count);
        printf("\n");
        printf("Installed memory   : %s\n", installed_buffer);
        printf("Physical used      : %s (%s)\n", physical_used_buffer, physical_used_pct_buffer);
        if (mem_info.swap_total_kb > 0)
        {
            char swap_total_buffer[32];
            format_kib(mem_info.swap_total_kb, swap_total_buffer, sizeof(swap_total_buffer));
            printf("Virtual used (swap): %s (%s of %s)\n", virtual_used_buffer, virtual_used_pct_buffer, swap_total_buffer);
        }
        else
        {
            printf("Virtual used (swap): %s (%s)\n", virtual_used_buffer, virtual_used_pct_buffer);
        }
        printf("\nCPU load per core  :\n");

        if (have_loads)
        {
            for (size_t i = 0; i < current_count; ++i)
            {
                printf("  cpu%-2zu -> %6.2f%%\n", i, loads[i]);
            }
        }
        else
        {
            printf("  Gathering data...\n");
        }

        fflush(stdout);

        free(loads);
        free(previous_times);
        previous_times = current_times;
        previous_count = current_count;

        sleep(REFRESH_INTERVAL_SECONDS);
    }

    free(previous_times);
    return exit_code;
}
