#include <avr/io.h>
#include <util/delay.h>

#include "switch.h"


void TheSwitch::begin(void)
{
	reg_init();
}

int TheSwitch::get(uint8_t address)
{
	int returnVal;
	reg_getRegister(address, &returnVal);
	return returnVal;
}


void TheSwitch::set(uint8_t address, int value)
{
	reg_setRegister(address, value);
}


void TheSwitch::set(uint8_t address, const char* string)
{
	int registerValue;
	
	while(1){
		registerValue = *string;
		if(*string++ == 0){
			reg_setRegister(address, registerValue);
			break;
		}
		registerValue |= (uint16_t)(*string) << 8;
		reg_setRegister(address++, registerValue);
		if(*string++ == 0){
			break;
		}	
	}
}


enum {
	menu_kLabel = 0,
	menu_kValue
};


void TheSwitch::update(void)
{
	reg_update();
	
	if(items == 0) return;
	
	// update state
	int key = get(Keypad);
	if(state == menu_kValue){
		if(key == CenterShort) state = menu_kLabel;
		if(menu[item].access == ReadWrite){
			if(key == LeftShort) *menu[item].value -= 1;
			if(key == LeftLong) *menu[item].value -= 10;
			if(key == RightShort) *menu[item].value += 1;
			if(key == RightLong) *menu[item].value += 10;
		}
	} else {
		if(key == LeftShort){
			if(item == 0) item = items;
			item--;
		}
		if(key == RightShort){
			item++;
			if(item == items) item = 0;
		}
		if(key == CenterShort) state = menu_kValue;	
		
	}
	
	// range check boolean
	if(menu[item].format == Bool){
		if(*menu[item].value > 1) *menu[item].value = 1;
		if(*menu[item].value < 0) *menu[item].value = 0;
	}
	
	// update display
	if(state == menu_kValue){
		int newValue = *(menu[item].value);
		if((newValue != lastValue) || (lastState == menu_kLabel)) {
			lastValue = newValue;
			lastState = menu_kValue;
			if(menu[item].access == ReadWrite){
				set(DisplayIntegerFormat, menu[item].format | Flash);
			} else {
				set(DisplayIntegerFormat, menu[item].format);
			}
			set(DisplayInteger, newValue);
		}
	} else {
		if((lastState == menu_kValue) || (lastItem != item)) {
			lastItem = item;
			lastState = menu_kLabel;
			set(DisplayStringFormat, menu[item].format);
			set(DisplayString,  menu[item].label);
		}
	}
}


void TheSwitch::addMenu(MenuItem* menuList)
{
	menu = menuList;
	item = 0;
	state = menu_kValue;
	lastState = menu_kLabel;	// force an initial update of the display
	lastValue = 0;
	lastItem = 0;
	
	while(menu[items].label != 0){
		items++;
	}	
}


TheSwitch Switch;

