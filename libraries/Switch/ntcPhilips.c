#include <avr/pgmspace.h>

#include "ntcPhilips.h"
#include "adc.h"

static int deciCelcius[2] = {0};

// philips / bc 2381-640-33103 with 4K7 drive resistor
#define TEMP_TABLE {	\
	1010, 	1004, 	997, 	988, 	976, 	962, 	943, 	921,	\
	895, 	864, 	828, 	788, 	744, 	697, 	647, 	596,	\
	544, 	493, 	445, 	398, 	355, 	315, 	278, 	245,	\
	216, 	190, 	167, 	147, 	129, 	113, 	100, 	88,		\
	78, 	69, 	61, 	54, 	48, 	43, 	38}

static const int16_t PROGMEM TEMP_LOOKUP_TABLE[] =  TEMP_TABLE;
#define TEMP_LOOKUP_TABLE_SIZE (sizeof(TEMP_LOOKUP_TABLE) / sizeof(int16_t))


static int16_t getDeciCelcius(int16_t rawADC)
{
	int16_t ref_lo, ref_hi, delta, temp;
	int8_t i;

	i = TEMP_LOOKUP_TABLE_SIZE;

	do {
        ref_lo = (int16_t)pgm_read_word(&TEMP_LOOKUP_TABLE[--i]);
    } while (((int16_t)rawADC > ref_lo) && (i > 1));

    ref_hi = (int16_t)pgm_read_word(&TEMP_LOOKUP_TABLE[--i]);
    delta = ref_hi - ref_lo;
    temp = -400 + (i + 1) * 50 - (50 * ((int16_t)rawADC - ref_lo)) / delta;

	return temp;
}


static int16_t getValue(uint8_t reference)
{
	uint8_t index = (reference & 0x01);
	return deciCelcius[index];
}


static void update(uint8_t reference)
{
	uint8_t index = (reference & 0x01);
	deciCelcius[index] = getDeciCelcius(adc()->getValue(reference));
}


HAL ntcPhilipsHAL = {
		0,
		update,
		getValue,
		0
};


HAL* ntcPhilips(void)
{
	return &ntcPhilipsHAL;
}
