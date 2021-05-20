#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <time.h>

#include "tinbus.h"

void log_begin(const char *path);
void log_commit(tinbus_frame_t* frame);
void log_end(void);

#endif
