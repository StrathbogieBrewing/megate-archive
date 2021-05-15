#include <stdbool.h>

#include "tinbus.h"

// CRC-8 uses DVB-S2 polynomial
unsigned char tinbus_crcByte(unsigned char crc, unsigned char data) {
  unsigned char i;
  for (i = 0x80; i > 0; i >>= 1) {
    bool bit = crc & 0x80;
    if (data & i)
      bit = !bit;
    crc <<= 1;
    if (bit)
      crc ^= 0xd5;
  }
  return crc;
}

unsigned char tinbus_crcFrame(const tinbus_frame_t *frame) {
  unsigned char crc = 0;
  unsigned const char *data = (unsigned char *)frame + 1; // no crc on start
  unsigned char bytes = tinbus_kFrameSize - 2; // or on crc
  while (bytes--) {
    crc = tinbus_crcByte(crc, *data++);
  }
  return crc;
}
