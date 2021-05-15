#include "msg.h"

void msg_pack(tinbus_frame_t *frame, msg_pack_t *pack, int value) {
  frame->msgID = pack->msgID;
  unsigned char byteOffset = pack->bitOffset >> 3;
  if (pack->bitCount == 8) {
    frame->data[byteOffset] = value;
  }
  if (pack->bitCount == 16) {
    frame->data[byteOffset] = (value >> 8);
    frame->data[byteOffset + 1] = value;
  }
}

int msg_unpack(tinbus_frame_t *frame, msg_pack_t *pack, int *value) {
  if(frame->msgID != pack->msgID){
    return MSG_NULL;
  }
  unsigned char byteOffset = pack->bitOffset >> 3;
  if (pack->bitCount == 8) {
      *value = (int)((signed char)frame->data[byteOffset]);
  } else if (pack->bitCount == 16) {
      *value = ((int)((signed char)frame->data[byteOffset]) << 8) +
              frame->data[byteOffset + 1];
  } else {
    return MSG_NULL;
  }
  return pack->format;
}
