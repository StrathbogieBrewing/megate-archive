#ifndef ITOA_H
#define ITOA_H

#include <stdint.h>

enum itoa_format {
	Unit = 0,
	Deci,
	Centi,
	Milli,
	Bool,
	TwoDigits,
	Time,
};

enum itoa_constants {
	itoa_kMaxStringLength = 8
};

uint8_t itoa_toString(char* str, int16_t value, uint8_t format);

#endif



