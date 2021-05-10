#include "msg.h"

void msg_pack(msg_message msg, msg_pack_t *pack, int value) {
  msg[0] = pack->msgID >> 8;
  msg[1] = pack->msgID;
  unsigned char dataByteOffset = pack->dataBitOffset >> 3;
  if (pack->dataBitCount == 8) {
    msg[dataByteOffset] = value;
  }
  if (pack->dataBitCount == 16) {
    msg[dataByteOffset] = (value >> 8);
    msg[dataByteOffset + 1] = value;
  }
}

int msg_unpack(msg_message msg, msg_pack_t *pack) {
  unsigned char dataByteOffset = pack->dataBitOffset >> 3;
  int value = 0;
  if (pack->dataBitCount == 8) {
      value = (int)((signed char)msg[dataByteOffset]);
  }
  if (pack->dataBitCount == 16) {
      value = ((int)((signed char)msg[dataByteOffset]) << 8) +
              msg[dataByteOffset + 1];
  }
  return value;
}
