#include <stdio.h>
#include <stdlib.h>

#include "sac.h"
#include "sacinterface.h"

static long long energy_uj(sac_int package, sac_int subzone)
{
    char path[128];
    if (subzone < 0) {
        snprintf(path, sizeof(path), "/sys/class/powercap/intel-rapl/intel-rapl:%" PRIisac "/energy_uj", package);
    } else {
        snprintf(path, sizeof(path), "/sys/class/powercap/intel-rapl/intel-rapl:%" PRIisac "/intel-rapl:%" PRIisac ":%" PRIisac "/energy_uj", package, package, subzone);
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
        return 0;
    }

    long long res;
    if (fscanf(fp, "%lld", &res) <= 0) {
        perror("fscanf");
        return 0;
    }

    if (fclose(fp) < 0) {
        perror("fclose");
    }

    return res;
}

static long long max_energy_range_uj(sac_int package, sac_int subzone)
{
    char path[128];
    if (subzone < 0) {
        snprintf(path, sizeof(path), "/sys/class/powercap/intel-rapl/intel-rapl:%" PRIisac "/max_energy_range_uj", package);
    } else {
        snprintf(path, sizeof(path), "/sys/class/powercap/intel-rapl/intel-rapl:%" PRIisac "/intel-rapl:%" PRIisac ":%" PRIisac "/max_energy_range_uj", package, package, subzone);
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("popen");
        return 0;
    }

    long long res;
    if (fscanf(fp, "%lld", &res) <= 0) {
        perror("fscanf");
        return 0;
    }

    if (fclose(fp) < 0) {
        perror("fclose");
    }

    return res;
}

long long raplStart(sac_int package, sac_int subzone)
{
    return energy_uj(package, subzone);
}

long long raplStop(sac_int package, sac_int subzone, long long energy_start)
{
    if (energy_start == 0) {
        return 0;
    }

    long long energy_end = energy_uj(package, subzone);

    if (energy_end >= energy_start) {
        return energy_end - energy_start;
    } else {
        // The accumulator overflowed
        long long accumulator_max = max_energy_range_uj(package, subzone);
        return energy_end + (accumulator_max - energy_start);
    }
}

long long powerLimit(void)
{
    char path[64] = "/sys/class/powercap/intel-rapl:0/constraint_0_power_limit_uw";

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return -1;
    }

    long long value;
    assert(fscanf(fp, "%lld", &value) > 0);
    fclose(fp);

    return value;
}

sac_int numThreads(void)
{
    // Plus one for the queen
    return (sac_int)(SAC_MT_cnt_worker_bees + 1);
}
