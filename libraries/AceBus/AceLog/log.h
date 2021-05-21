#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <time.h>

#include "tinframe.h"

void log_begin(const char *path);
void log_commit(tinframe_t* frame);
void log_end(void);

#endif
