#ifndef TINUX_H
#define TINUX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tinbus.h"

int tinux_open(char *port);
int tinux_read(tinbus_frame_t *rxFrame);
void tinux_close(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINUX_H
