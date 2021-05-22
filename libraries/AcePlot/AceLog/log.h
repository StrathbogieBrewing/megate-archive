#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <time.h>



bool log_begin(const char *path);
bool log_read(const char *sources, time_t startSecond, time_t endSecond);
// bool log_readSourceList(time_t startSecond, time_t endSecond);
// bool plot_end(void);

#endif // LOG_H
