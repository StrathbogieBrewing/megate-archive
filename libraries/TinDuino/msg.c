#include <string.h>

#include "msg.h"

void msg_pack(tinframe_t *frame, msg_pack_t *pack, int value) {
  frame->data[MSG_ID] = pack->msgID;
  unsigned char byteOffset = pack->bitOffset >> 3;
  if (pack->bitCount == 8) {
    frame->data[MSG_DATA + byteOffset] = value;
  }
  if (pack->bitCount == 16) {
    frame->data[MSG_DATA + byteOffset] = (value >> 8);
    frame->data[MSG_DATA + byteOffset + 1] = value;
  }
}

int msg_unpack(tinframe_t *frame, const msg_pack_t *pack, int *value) {
  if (frame->data[MSG_ID] != pack->msgID) {
    return MSG_NULL;
  }
  unsigned char byteOffset = pack->bitOffset >> 3;
  if (pack->bitCount == 8) {
    *value = (int)((signed char)frame->data[MSG_DATA + byteOffset]);
  } else if (pack->bitCount == 16) {
    *value = ((int)((signed char)frame->data[MSG_DATA + byteOffset]) << 8) +
             frame->data[MSG_DATA + byteOffset + 1];
  } else {
    return MSG_NULL;
  }
  return pack->format;
}

int _itoa(char *str, unsigned char format, int value) {
  unsigned char digit;
  unsigned char enable_digits = 0;
  char *ptr;
  ptr = str;
  // do sign
  if (value < 0) {
    value = -value;
    *str++ = '-';
  }
  // do 10000's
  digit = '0';
  while (value > 9999) {
    value -= 10000;
    digit++;
  }
  if ((digit != '0') || (enable_digits != 0)) {
    *str++ = digit;
    enable_digits = 1;
  }
  // do 1000's
  digit = '0';
  while (value > 999) {
    value -= 1000;
    digit++;
  }
  if ((digit != '0') || (enable_digits != 0)) {
    *str++ = digit;
    enable_digits = 1;
  }
  // do 100's
  digit = '0';
  while (value > 99) {
    value -= 100;
    digit++;
  }
  if (format == MSG_MILLI) {
    if (enable_digits == 0)
      *str++ = '0';
    *str++ = '.';
    enable_digits = 1;
  }
  if ((digit != '0') || (enable_digits != 0)) {
    *str++ = digit;
    enable_digits = 1;
  }
  // do 10's
  digit = '0';
  while (value > 9) {
    value -= 10;
    digit++;
  }
  if (format == MSG_CENTI) {
    if (enable_digits == 0)
      *str++ = '0';
    *str++ = '.';
    enable_digits = 1;
  }
  if ((digit != '0') || (enable_digits != 0) || (format == MSG_TWODIGITS)) {
    *str++ = digit;
    enable_digits = 1;
  }
  // do 1's
  digit = '0' + value;
  if (format == MSG_DECI) {
    if (enable_digits == 0)
      *str++ = '0';
    *str++ = '.';
  }
  *str++ = digit;
  *str = 0;

  return str - ptr;
}

int msg_format(tinframe_t *frame, const msg_pack_t *pack, char *str) {
  int value;
  unsigned char format = msg_unpack(frame, pack, &value);

  if (format == MSG_TIME) {
    int hours = 0;
    while (value >= 60) {
      ++hours;
      value -= 60;
    }
    str += _itoa(str, MSG_TWODIGITS, hours);
    *str++ = ':';
    _itoa(str, MSG_TWODIGITS, value);
  } else if (format == MSG_BOOL) {
    if (value)
      strcpy(str, "on");
    else
      strcpy(str, "off");
  } else {
    _itoa(str, format, value);
  }
  return strlen(str);
}
