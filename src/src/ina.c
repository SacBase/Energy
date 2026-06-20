#include <stdio.h>
#include <stdlib.h>

#include "sacinterface.h"

/**
 * For reading whole-system energy consumption statistics, provided by the INA260 chip available on the Software Energy Lab runners.
 *
 * The received json looks as follows:
 * ```json
 * {"electricity_consumed_current":%f,"measurements":%d,"steady_time":%d,"electricity_consumed_total":%f,"power_draw":%f}
 * ```
 */
static double inaRead(void)
{
    const char *url = getenv("ENERGY_STATS");
    if (!url) {
        return 0.0;
    }

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "curl -s \"%s\"", url);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("popen");
        return 0.0;
    }

    char buf[2048];
    double current, total, power;
    int measurements, steady_time;
    double res = 0.0;

    if (fgets(buf, sizeof(buf), fp)) {
        int n = sscanf(buf,
            " { \"electricity_consumed_current\" : %lf ,"
            " \"measurements\" : %d ,"
            " \"steady_time\" : %d ,"
            " \"electricity_consumed_total\" : %lf ,"
            " \"power_draw\" : %lf } %*[^}]",
            &current, &measurements, &steady_time, &total, &power
        );

        if (n == 5) {
            res = total;
        } else {
            fprintf(stderr, "Failed to parse ENERGY_STATS response\n");
        }
    }

    if (pclose(fp) < 0) {
        perror("pclose");
    }

    return res;
}

double inaStart(void)
{
    return inaRead();
}

double inaStop(double start)
{
    double end = inaRead();
    return end - start;
}
