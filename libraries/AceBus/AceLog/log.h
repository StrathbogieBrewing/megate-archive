#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <time.h>

void log_begin(const char *logPath, int dataSize);
void log_commit(void* data);
void log_end(void);


uint32_t log_read(uint32_t time, uint32_t day, void* data);

#endif
