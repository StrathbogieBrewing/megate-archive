#include <ctype.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "log.h"
#include "mkdir.h"

// maximum log record string
#define kMaxStrLen (1024)

// maximum log file buffer size
#define kLogBufferSize (1048576)

typedef struct {
  uint32_t timestamp;
  tinframe_t frame;
} log_t;

static log_t logBuffer[kLogBufferSize];
static long logBufferIndex = 0;
static time_t logBufferTime = 0;

#define kDirStrLen (kMaxStrLen / 2)
#define kBaseStrLen (kMaxStrLen / 4)

static char basePath[kBaseStrLen] = {0};

static bool isSameMinute(time_t t1, time_t t2) {
  struct tm tm1 = *gmtime(&t1);
  struct tm tm2 = *gmtime(&t2);
  if (tm1.tm_min == tm2.tm_min)
    return true;
  return false;
}

void writeToDisk(time_t tv_secs) {
  if (logBufferIndex == 0)
    return;
  struct tm tm = *gmtime(&tv_secs);
  // path
  char directory[kDirStrLen];
  sprintf(directory, "%s%4.4lu/%2.2u/%2.2u", basePath, 1900L + tm.tm_year,
          tm.tm_mon + 1, tm.tm_mday);
  build(directory);

  // file name
  char destination[kMaxStrLen];
  sprintf(destination, "%s/%2.2u.dat", directory, tm.tm_hour);

  // write / append buffer to file
  FILE *fd = fopen(destination, "a");
  if (fd != NULL) {
    fwrite(logBuffer, sizeof(log_t), logBufferIndex, fd);
    fclose(fd);
    logBufferIndex = 0;
  }
}

void log_begin(const char *path) {
  logBufferIndex = 0;
  logBufferTime = 0;
  int length = strnlen(path, kMaxStrLen);
  strncpy(basePath, path, kBaseStrLen - 1);
  if (length) {
    if (basePath[length - 1] != '/')
      strncat(basePath, "/", kBaseStrLen - 1);
  }
}

static inline void appendToLogBuffer(log_t *log) {
  memcpy(&logBuffer[logBufferIndex], log, sizeof(log_t));
  if (++logBufferIndex >= kLogBufferSize)
    logBufferIndex--;
}

void log_commit(tinframe_t* frame) {
  // get millisecond timestamp for this log commit
  struct timeval tv;
  gettimeofday(&tv, NULL);
  time_t secondsNow = tv.tv_sec;
  struct tm tm = *gmtime(&secondsNow);
  uint32_t millisNow = (uint32_t)tv.tv_usec / 1000L +
                       (uint32_t)(tm.tm_min * 60 + tm.tm_sec) * 1000LL;

  if (logBufferTime == 0)
    logBufferTime = secondsNow;

  if (!isSameMinute(logBufferTime, secondsNow)) {
    writeToDisk(logBufferTime);
    logBufferTime = secondsNow;
  }

  log_t log;  // add timestamp to frame and append to log
  log.timestamp = htonl(millisNow);
  memcpy(&log.frame, frame, tinframe_kFrameSize);
  appendToLogBuffer(&log);
}

void log_end(void) {
  if (logBufferIndex != 0) {
    writeToDisk(logBufferTime);
  }
}
