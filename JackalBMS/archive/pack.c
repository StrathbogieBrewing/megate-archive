#include "pack.h"

int32_t pack_extract(uint16_t reg, pack_t packing) {
  if (packing <= PACK_U1B15)
    return (reg >> packing) & 0x0001;

  if (packing == PACK_U2B0)
    return reg & 0x0003;

  if (packing == PACK_U2B6)
    return (reg >> 6) & 0x0003;

  if (packing == PACK_U4B12)
    return (reg >> 12) & 0x000F;
  if (packing == PACK_U4B8)
    return (reg >> 8) & 0x000F;
  if (packing == PACK_U4B4)
    return (reg >> 4) & 0x000F;
  if (packing == PACK_U4B0)
    return reg & 0x000F;

  if (packing == PACK_U8B0)
    return reg & 0x00FF;
  if (packing == PACK_U8B8)
    return (reg >> 8);
  if (packing == PACK_S8B0)
    return (int32_t)((int8_t)(reg & 0x00FF));
  if (packing == PACK_S8B8)
    return (int32_t)((int8_t)(reg >> 8));
  if (packing == PACK_S16)
    return (int32_t)((int16_t)reg);
  return reg;
}

uint16_t pack_insert(uint16_t reg, pack_t packing, int32_t val) {
  if (packing <= PACK_U1B15) {
    uint16_t mask = 1 << packing;
    if (val) {
      reg |= mask;
    } else {
      reg &= ~mask;
    }
  }

  if (packing == PACK_U2B0) {
    val &= 0x0003;
    reg &= 0xFFFC;
    reg |= val;
  }

  if (packing == PACK_U4B0) {
    val &= 0x000F;
    reg &= 0xFFF0;
    reg |= val;
  }

  if (packing == PACK_U4B4) {
    val &= 0x000F;
    val <<= 4;
    reg &= 0xFF0F;
    reg |= val;
  }

  if (packing == PACK_U4B8) {
    val &= 0x000F;
    val <<= 8;
    reg &= 0xF0FF;
    reg |= val;
  }

  if (packing == PACK_U4B12) {
    val &= 0x000F;
    val <<= 12;
    reg &= 0x0FFF;
    reg |= val;
  }

  if ((packing == PACK_U8B0) || (packing == PACK_S8B0)) {
    val &= 0x00FF;
    reg &= 0xFF00;
    reg |= val;
  }

  if ((packing == PACK_U8B8) || (packing == PACK_S8B8)) {
    val &= 0x00FF;
    val <<= 8;
    reg &= 0x00FF;
    reg |= val;
  }

  return reg;
}

int32_t pack_signExtend(uint16_t reg, pack_t packing) {
  if ((packing == PACK_S8B0) || (packing == PACK_S8B8))
    return (int32_t)((int8_t)(reg & 0x00FF));
  if (packing == PACK_S16)
    return (int32_t)((int16_t)reg);
  return reg;
}
