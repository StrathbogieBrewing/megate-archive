#ifndef PLOT_H
#define PLOT_H

#include <stdbool.h>
#include <time.h>

// #include "tinbus.h"

bool plot_begin(const char *path);
bool plot_read(const char *sources, time_t startSecond, time_t endSecond);
// bool log_readSourceList(time_t startSecond, time_t endSecond);
// bool plot_end(void);

#endif // PLOT_H
