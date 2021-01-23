#ifndef ASM_H
#define ASM_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

uint16_t asm_htons(uint16_t num);
uint8_t asm_hi(uint16_t num);
uint8_t asm_lo(uint16_t num);

#if defined(__cplusplus)
}
#endif

#endif
