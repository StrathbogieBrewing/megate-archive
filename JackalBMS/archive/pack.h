#ifndef PACK_H
#define PACK_H

#include <stdint.h>

typedef enum {
  BITOFFSET_0 = 0x00,
  BITOFFSET_1 = 0x01,
  BITOFFSET_2 = 0x02,
  BITOFFSET_3 = 0x03,
  BITOFFSET_4 = 0x04,
  BITOFFSET_5 = 0x05,
  BITOFFSET_6 = 0x06,
  BITOFFSET_7 = 0x07,
  BITOFFSET_8 =  0x08,
  BITOFFSET_12 = 0x09,
  BITOFFSET_16 = 0x0A,
  BITOFFSET_24 = 0x0B,
  BITOFFSET_32 = 0x0C,
  BITOFFSET_40 = 0x0D,
  BITOFFSET_48 = 0x0E,
  BITOFFSET_56 = 0x0F,
};

typedef enum {
  BITCOUNT_1 = 0x00,
  BITCOUNT_4 = 0x10,
  BITCOUNT_8 = 0x20,
  BITCOUNT_16 = 0x30
};

typedef enum {
  UNSIGNED = 0x00,
  SIGNED = 0x80
};



// Packing
typedef enum {
  PACK_U1B0 = 0,
  PACK_U1B1,
  PACK_U1B2,
  PACK_U1B3,
  PACK_U1B4,
  PACK_U1B5,
  PACK_U1B6,
  PACK_U1B7,
  PACK_U1B8,
  PACK_U1B9,
  PACK_U1B10,
  PACK_U1B11,
  PACK_U1B12,
  PACK_U1B13,
  PACK_U1B14,
  PACK_U1B15,

  PACK_U2B0,
  PACK_U2B6,

  PACK_U4B0,
  PACK_U4B4,
  PACK_U4B8,
  PACK_U4B12,

  PACK_U8B0,
  PACK_U8B8,
  PACK_S8B0,
  PACK_S8B8,

  PACK_U16,
  PACK_S16,

  PACK_NONE,
} pack_t;

uint16_t pack_insert(uint16_t reg, pack_t packing, int32_t value);
int32_t pack_extract(uint16_t reg, pack_t packing);
int32_t pack_signExtend(uint16_t reg, pack_t packing);

#endif // PACK_H
