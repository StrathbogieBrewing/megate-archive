#include <avr/interrupt.h>
#include "bios.h"
#include "rtc.h"


static int16_t rtc_get(uint8_t reference)
{
	cli();
	uint16_t minutes = (uint16_t)bios_minutes + 60 * (uint16_t)bios_hours;
	sei();
	return minutes;
}

static void rtc_set(uint8_t reference, int16_t value)
{
	cli();
	bios_minutes = value % 60;
	bios_hours = value / 60;
	sei();
}

HAL rtcHAL = {
		0,
		0,
		rtc_get,
		rtc_set
};

HAL* rtc(void)
{
	return &rtcHAL;
}
