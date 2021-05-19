#ifndef MSG_H
#define MSG_H

#include "tinbus.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
  MSG_NULL = 0,
  MSG_MILLI,     // make fixed point with 3 decimal places
  MSG_CENTI,     // make fixed point with 2 decimal places
  MSG_DECI,      // make fixed point with 1 decimal places
  MSG_UNIT,      // display at least one digit
  MSG_TWODIGITS, // display at least two digit
  MSG_TIME,      // display as a time
  MSG_BOOL,      // display as on / off
};

typedef struct {
  const unsigned char msgID;
  const unsigned char bitOffset;
  const unsigned char bitCount;
  const unsigned char format;
} msg_pack_t;

typedef struct {
  const char *name;
  const msg_pack_t pack;
} msg_name_t;

void msg_pack(tinbus_frame_t *frame, msg_pack_t *pack, int value);
int msg_unpack(tinbus_frame_t *frame, const msg_pack_t *pack, int *value);

int msg_format(tinbus_frame_t *frame, const msg_pack_t *pack, char *str);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MESSAGE_H
