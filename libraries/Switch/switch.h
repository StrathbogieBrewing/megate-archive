#ifndef SWITCH_H
#define SWITCH_H

#include <stdint.h>

#include "reg.h"


enum {
	ReadOnly = 0,
	ReadWrite
};


typedef struct {
	const char* label;
	uint8_t format;
	uint8_t access;
	int* value;
} MenuItem;


class TheSwitch {

public:
	void begin(void);
	void update(void);
	int16_t get(uint8_t address);
	void set(uint8_t address, int16_t value);
	void set(uint8_t address, const char* string);
	void addMenu(MenuItem* menuList);
	
private:
	uint8_t item;
	uint8_t items;
	uint8_t state;
	uint8_t lastState;
	uint8_t lastItem;
	int16_t lastValue;
	MenuItem* menu;
};

extern TheSwitch Switch;

#endif
