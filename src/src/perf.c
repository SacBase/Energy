#define _GNU_SOURCE
#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sacinterface.h"

unsigned int perfTypeHardware(void) { return PERF_TYPE_HARDWARE; }
unsigned int perfTypeSoftware(void) { return PERF_TYPE_SOFTWARE; }
unsigned long perfCounterInsns(void) { return PERF_COUNT_HW_INSTRUCTIONS; }
unsigned long perfCounterCycles(void) { return PERF_COUNT_HW_CPU_CYCLES; }

sac_int perfEventCreate(unsigned int type, unsigned long config, sac_int pid, sac_int cpu)
{
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = type;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = config;
    pe.disabled = 1;
    pe.exclude_kernel = 0;
    pe.exclude_hv = 0;

    int fd = syscall(__NR_perf_event_open, &pe, (pid_t)pid, (int)cpu, -1, 0);
    if (fd == -1) {
        return -1;
    }

    if (ioctl(fd, PERF_EVENT_IOC_ENABLE, 0) < 0) {
        perror("perf_event_enable");
        return -1;
    }

    return (sac_int)fd;
}

sac_int perfEventStop(sac_int fd)
{
    if (ioctl((int)fd, PERF_EVENT_IOC_DISABLE, 0) < 0) {
        perror("perf_event_disable");
        return -1;
    }

    if (close((int)fd) < 0) {
        perror("perf_event_close");
        return -1;
    }

    return 0;
}

sac_int perfEventReset(sac_int fd)
{
    if (ioctl((int)fd, PERF_EVENT_IOC_RESET, 0) < 0) {
        perror("perf_event_reset");
        return -1;
    }
    return 0;
}

unsigned long perfEventRead(sac_int fd)
{
    unsigned long res;
    if (read((int)fd, &res, sizeof(unsigned long))) {
        perror("perf_event_read");
        return 0;
    }
    return res;
}
