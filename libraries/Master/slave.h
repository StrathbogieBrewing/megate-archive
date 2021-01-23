#ifndef SLAVE_H
#define SLAVE_H

#include <stdint.h>
#include <stdio.h>

#include "../Switch/switch.h"

class Slave {

public:
	Slave();
	void begin(uint8_t address);
	int get(uint8_t address);
	void set(uint8_t address, int value);
	void set(uint8_t address, char* string);	
	
	void addMenu(MenuItem* menuList);
	void update(void);

private:
	uint8_t getRegisters(uint16_t* data, uint16_t address, uint8_t quantity);
	uint8_t setRegisters(uint16_t* data, uint16_t address, uint8_t quantity);
	uint8_t slaveAddress;
	uint8_t format;
	
	uint8_t item;
	uint8_t items;
	uint8_t state;
	uint8_t lastState;
	uint8_t lastItem;
	int16_t lastValue;
	MenuItem* menu;
};

#endif
