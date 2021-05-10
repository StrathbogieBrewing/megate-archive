#ifndef MSG_H
#define MSG_H

enum {
  MSG_MILLI = 0, // make fixed point with 3 decimal places
  MSG_CENTI,     // make fixed point with 2 decimal places
  MSG_DECI,      // make fixed point with 1 decimal places
  MSG_UNIT,      // display at least one digit
  MSG_TWODIGITS, // display at least two digit
  MSG_TIME,      // display as a time
  MSG_BOOL,      // display as on / off
};

typedef struct {
  const unsigned int  msgID;
  const unsigned char dataBitOffset;
  const unsigned char dataBitCount;
  const unsigned char dataFormat;
} msg_pack_t;

typedef unsigned char msg_message[10];

void msg_pack(msg_message msg, msg_pack_t *pack, int value);
int msg_unpack(msg_message msg, msg_pack_t *pack);





#endif // MESSAGE_H
