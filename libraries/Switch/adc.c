#include <avr/io.h>
#include <avr/interrupt.h>

#include "bios.h"
#include "adc.h"

#define kHighThreshold 768
#define kLowThreshold 256
#define kCounterOffset 2

static volatile uint16_t adc_result[10] = {0};
static volatile uint8_t adc_channel = 0;


static void init(uint8_t reference)
{
	PORTC |= (1 << 2);					// turn on auxiliary power to VADC
	DDRC |= (1 << 2);

	adc_channel = 0;
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (0 << ADPS0) | (1 << ADIE);

	ADMUX = adc_channel | (1 << REFS0);	// AVCC for AREF and ADCx
	ADCSRA |= (1 << ADSC);				// start conversion

	sei();	// enable interrupts

	while(adc_channel == 0);			// wait for first conversions to complete
	while(adc_channel == 1);
}


static int16_t getValue(uint8_t reference)
{
	uint8_t sreg;
	int16_t value;

	reference--;
	reference ^= 0x01;
	
	uint8_t clear = 0;
	if((reference == 2) || (reference == 3)) clear = 1;

	sreg = SREG;
	cli();
	value = adc_result[reference & 0x07];
	if(clear){
		adc_result[reference & 0x07] = 0;
	}
	SREG = sreg;
	return value;
}


ISR(ADC_vect)
{
	static uint8_t seconds = 0;

	volatile uint16_t* result = &adc_result[0];


	if(adc_channel != 0){
		result++;
	}

	*result = ADC;

	if((*(result + 8) & 0x01) == 0){
		if(*result > kHighThreshold){
		 (*(result + 8))++;
		 }
	} else {
		if(*result < kLowThreshold){
		 (*(result + 8))++;
		 (*(result + 2))++;
		 }
	}

	if(seconds != bios_seconds){
		seconds = bios_seconds;
		adc_result[4] = (adc_result[8] - adc_result[6]) >> 1;
		adc_result[5] = (adc_result[9] - adc_result[7]) >> 1;
		adc_result[6] = adc_result[8];
		adc_result[7] = adc_result[9];
	}

	adc_channel ^= 0x01;
	ADMUX = adc_channel | (1 << REFS0); 		// AVCC for AREF and ADCx
	ADCSRA |= (1 << ADSC);        				// start next conversion
}


HAL adcHAL = {
	init,
	0,
	getValue,
	0
};


HAL* adc(void)
{
	return &adcHAL;
}
