#include <string.h>

#include <avr/io.h>
#include <avr/eeprom.h>

#include "reg.h"

#include "bios.h"
#include "seg.h"


static uint8_t reg_mapSize = 0;

typedef const struct {
	getHAL 	device;
	uint8_t reference;
} Register;


Register reg_map[] = {
	{pwm, pwm_kOutput1},
	{pwm, pwm_kOutput2},
	{adc, adc_kInput1},
	{adc, adc_kInput2},
	{i2c, i2c_kInput},
	{i2c, i2c_kInputNoGain},
	{ntcPhilips, adc_kInput1},
	{ntcPhilips, adc_kInput2},
	{ntcHoneywell, adc_kInput1},
	{ntcHoneywell, adc_kInput2},
	{adc, adc_kCounter1},
	{adc, adc_kCounter2},
	{adc, adc_kFrequency1},
	{adc, adc_kFrequency2},
	{uif, uif_kKeypad},
	{uif, uif_kDisplayFormat},
	{uif, uif_kDisplayInteger},
	{uif, uif_kDisplayFormat},
	{uif, uif_kDisplayString1},
	{uif, uif_kDisplayString2},
	{uif, uif_kDisplayString3},
	{uif, uif_kDisplayString4},
	{ee, ee_SlaveAddress},
	{ee, ee_eeprom1},
	{ee, ee_eeprom2},
	{ee, ee_eeprom3},
	{rtc, 0},
	{0},
};


uint8_t reg_init(void)
{
	Register* i = reg_map;
	reg_mapSize = 0;

	while(*i->device){
		initFunction initFunc = i->device()->init;
		if(initFunc) initFunc(i->reference);
		reg_mapSize++;
		i++;
	}

	return reg_kOK;
}


uint8_t reg_setRegister(uint8_t registerAddress, int16_t registerValue)
{
	if(registerAddress >= reg_mapSize) return reg_kAddressInvalid;

	Register* i = &reg_map[registerAddress];
	setValueFunction setFunc = i->device()->setValue;

	if(setFunc){
		setFunc(i->reference, registerValue);
	} else {
		return reg_kAddressInvalid;
	}

	return reg_kOK;
}


uint8_t reg_getRegister(uint8_t registerAddress, int16_t* registerValue)
{
	if(registerAddress >= reg_mapSize) return reg_kAddressInvalid;

	Register* i = &reg_map[registerAddress];
	getValueFunction getFunc = i->device()->getValue;

	if(getFunc){
		*registerValue = getFunc(i->reference);
	} else {
		return reg_kAddressInvalid;
	}

	return reg_kOK;
}


uint8_t reg_update(void)
{
	static uint8_t ticks = 0;

	// limit update to every 1/4 second
	uint8_t newticks = bios_ticks & 0b11000000;
	
	if(ticks != newticks){
		ticks = newticks;
		Register* i = reg_map;
		while(*i->device){
			updateFunction updateFunc = i->device()->update;
			if(updateFunc) updateFunc(i->reference);
			i++;
		}
	}
	return reg_kOK;
}

