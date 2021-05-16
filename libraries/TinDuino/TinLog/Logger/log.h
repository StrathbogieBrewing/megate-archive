#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <time.h>

#include "tinbus.h"

// maximum log record string
#define kMaxStrLen (1024)

// maximum log file buffer size
#define kFileBufferSize (1048576)

#define kMaxQueryCount (16)

bool log_initialise(const char *path);
bool log_commit(tinbus_frame_t* frame);
bool log_read(const char *sources, time_t startSecond, time_t endSecond);
bool log_readSourceList(time_t startSecond, time_t endSecond);
bool log_terminate(void);

#endif
