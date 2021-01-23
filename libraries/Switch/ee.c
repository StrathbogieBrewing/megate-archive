#include <avr/eeprom.h>

#include "ee.h"


static int16_t getValue(uint8_t reference)
{
	return (int16_t)eeprom_read_word((uint16_t*)((uint16_t)reference << 1));
}

static void setValue(uint8_t reference, int16_t value)
{
	eeprom_write_word((uint16_t*)((uint16_t)reference << 1), (uint16_t)value);
}


HAL eeHAL = {
	0,
	0,
	getValue,
	setValue
};


HAL* ee(void)
{
	return &eeHAL;
}
