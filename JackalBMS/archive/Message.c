#include "message.h"



typedef struct {
  char const *itemName;
  const unsigned char dataBitOffset;
  const unsigned char dataBitCount;
  const unsigned char
      dataType; // signed / unsigned / milli / deci /centi / kilo / time
} dataItem_t;

const pack_t batv = {0, 16, PACK_UNSIGNED_MILLI};

void pack_insert(unsigned char msg[], pack_t *pack, unsigned int value) {
  unsigned char dataByteOffset = pack->dataBitOffset >> 3;
  // unsigned char dataByteCount = pack->dataBitCount >> 3;

  if (pack->dataBitCount == 8) {
    msg[dataByteOffset] = (unsigned char)value;
  }

  if (pack->dataBitCount == 8) {
    msg[dataByteOffset] = (unsigned char)value;
  }

  while (dataByteCount > 0) {
    dataByteCount--;
    msg[dataByteOffset + dataByteCount] = (unsigned char)value;
    value >>= 8;
  }
}

unsigned int pack_extract(unsigned char msg[], pack_t *pack) {
  unsigned char dataByteOffset = pack->dataBitOffset >> 3;
  unsigned int value = 0;

  if (pack->dataBitCount == 8) {
    if (pack->dataType & PACK_SIGNED) {
      value = (int)msg[dataByteOffset];
    } else {
      value = (unsigned int)msg[dataByteOffset];
    }
  }

  if (pack->dataBitCount == 16) {
    if (pack->dataType & PACK_SIGNED) {
      value = (int)(((unsigned int)msg[dataByteOffset] << 8) +
              (unsigned int)msg[dataByteOffset + 1]);
    } else {
      value = ((unsigned int)msg[dataByteOffset] << 8) +
              (unsigned int)msg[dataByteOffset + 1];
    }
  }
  return value;
}

void pack_format(unsigned int value, pack_t *pack) {}

dataItem_t bmsItems[] = {
    // name, bit offset, bit count, format
    {"Battery Voltage", 0, 16, PACK_UNSIGNED_MILLI},
    {"Battery Current", 16, 16, PACK_SIGNED_CENTI},
    {"Battery Target Voltage", 32, 8, PACK_UNSIGNED_MILLI},
    {"Battery Target Current", 40, 8, PACK_SIGNED_UNIT},
    {"Cell Voltage Range", 48, 8, PACK_UNSIGNED_MILLI},
    {"Battery Temperature", 56, 8, PACK_SIGNED_UNIT},
};

#define kDataCount (sizeof(bmsItems) / sizeof(dataItem_t))

bool msg_register(uint16_t msgID, uint8_t dataCount, dataItem_t *dataItems);
