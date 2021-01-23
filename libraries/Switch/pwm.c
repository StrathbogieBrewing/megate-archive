#include <avr/io.h>

#include "pwm.h"


static void init(uint8_t reference)
{
	DDRB |= (1 << PORTB2) | (1 << PORTB1);
	TCCR1B = (1 << CS11) | (1 << CS10);
	TCCR1A = (1 << WGM10);
}


static void setValue(uint8_t reference, int16_t value)
{
	if(reference == pwm_kOutput1){
		if(value <= 0){
			TCCR1A &= ~(1 << COM1B1);
			PORTB &= ~(1 << PORTB2);
		} else if(value >= 255){
			TCCR1A &= ~(1 << COM1B1);
			PORTB |= (1 << PORTB2);
		} else {
			TCCR1A |= (1 << COM1B1);
			OCR1B = value;
		}
	}

	if(reference == pwm_kOutput2){
		if(value <= 0){
			TCCR1A &= ~(1 << COM1A1);
			PORTB &= ~(1 << PORTB1);
		} else if(value >= 255){
			TCCR1A &= ~(1 << COM1A1);
			PORTB |= (1 << PORTB1);
		} else {
			TCCR1A |= (1 << COM1A1);
			OCR1A = value;
		}
	}

	return;
}


HAL pwmHAL = {
	init,
	0,
	0,
	setValue
};


HAL* pwm(void)
{
	return &pwmHAL;
}
