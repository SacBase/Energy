#include <stdio.h>
#include <stdlib.h>

#include "sacinterface.h"

int getTemperature(sac_int zone)
{
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/thermal/thermal_zone%" PRIisac "/temp", zone);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        return 0;
    }

    int res;
    if (fscanf(fp, "%d", &res) <= 0) {
        perror("fscanf");
        return 0;
    }

    if (fclose(fp) < 0) {
        perror("fclose");
    }

    return res;
}
