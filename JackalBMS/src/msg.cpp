#include "msg.h"

void msg_pack(msg_message msg, msg_pack_t *pack, int value) {
  msg[0] = pack->msgID >> 8;
  msg[1] = pack->msgID;
  unsigned char dataByteOffset = pack->dataBitOffset >> 3;
  if (pack->dataBitCount == 8) {
    msg[dataByteOffset + 2] = value;
  }
  if (pack->dataBitCount == 16) {
    msg[dataByteOffset + 2] = (value >> 8);
    msg[dataByteOffset + 3] = value;
  }
}

bool msg_unpack(msg_message msg, msg_pack_t *pack, int *value) {
  if(msg[0] != pack->msgID >> 8) || (msg[1] = pack->msgID)){
    return false;
  }
  unsigned char dataByteOffset = pack->dataBitOffset >> 3;
  if (pack->dataBitCount == 8) {
      *value = (int)((signed char)msg[dataByteOffset + 2]);
  } else if (pack->dataBitCount == 16) {
      *value = ((int)((signed char)msg[dataByteOffset + 2]) << 8) +
              msg[dataByteOffset + 3];
  } else {
    return false;
  }
  return true;
}
