#include <avr/io.h>
#include <util/crc16.h>
#include <util/delay.h>

#include "slave.h"
#include "asm.h"

#define kRXTimeout 100
#define kCharTimeout 30

#define kSlaveAddressOffset 0
#define kFunctionCodeOffset 1
#define kRegisterAddressOffset 2
#define kRegisterQuantityOffset 4

#define kReadDataOffset 3
#define kWriteDataOffset 7

#define kFunctionReadRegisters 3
#define kFunctionWriteRegisters 16

#define kBufferSize 32

#define kOK 0
#define kOverrunError 1
#define kCRCError 3
#define kTimeoutError 4
#define kFrameError 5


uint16_t crc(uint8_t* data, uint8_t bytes)
{
	uint16_t CRC = 0xFFFF;

	while(bytes--){
		CRC = _crc16_update(CRC, *data++);
	}
	return CRC;
}


void tx(uint8_t* buffer, uint8_t bytes)
{
	// init uart uart 19200 8 n 1
	UCSRB = (1 << TXEN);	
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	UBRRL = 25;
	UCSRA |= (1 << TXC);

	for(uint8_t i = 0 ; i < bytes ; i++){
		while(!( UCSRA & (1 << UDRE)));
		UDR = buffer[i];
	}
	while(!(UCSRA & (1 << TXC)));

	UCSRB |= (1 << RXEN);
}


uint8_t rx(uint8_t* buffer, uint8_t* bytes)
{
	*bytes = 0;
	uint8_t timer = kRXTimeout;
	while(*bytes < kBufferSize){
		while (!(UCSRA & (1 << RXC))){
			if(timer) --timer;
			if((timer == 0) && (*bytes)) return kOK;
			if(timer == 0) return kTimeoutError;
			_delay_us(50);
		}
		timer = kCharTimeout;
		buffer[*bytes] = UDR;
		*bytes = *bytes + 1;
	}
	return kOverrunError;
}


void send(uint8_t* buffer, uint8_t bytes)
{
	if(bytes + 2 < kBufferSize){
		uint16_t CRC = crc(buffer, bytes);	
		buffer[bytes] = CRC & 0xFF;
		buffer[bytes + 1] = (CRC >> 8) & 0xFF;
		tx(buffer, bytes + 2);
	}
}


uint8_t receive(uint8_t* buffer, uint8_t* bytes)
{
	uint8_t error, byteCount;
	
	*bytes = 0;
	error = rx(buffer, &byteCount);
	if(error) return error;
	if(byteCount < 4) return kFrameError;
	uint16_t CRC = crc(buffer, byteCount);
	if(CRC) return kCRCError;
	*bytes = byteCount - 2;
	return kOK;
}


Slave::Slave()
{
	items = 0;
}

uint8_t Slave::getRegisters(uint16_t* data, uint16_t address, uint8_t quantity)
{
	uint8_t buffer[kBufferSize];
	// build frame
	buffer[kSlaveAddressOffset] = slaveAddress;
	buffer[kFunctionCodeOffset] = kFunctionReadRegisters;
	*(uint16_t*)(&buffer[kRegisterAddressOffset]) = asm_htons(address);
	*(uint16_t*)(&buffer[kRegisterQuantityOffset]) = asm_htons(quantity);
	// send frame
	send(buffer, kRegisterQuantityOffset + 2);
	// receive response
	uint8_t rxBytes;
	uint8_t error = receive(buffer, &rxBytes);
	if(error != kOK) return error;
	// extract data from frame
	for(uint8_t i = 0 ; i < quantity ; i++){
		data[i] = asm_htons(*(uint16_t*)(&buffer[kReadDataOffset + i * 2]));
	}	
	return kOK;
}


uint8_t Slave::setRegisters(uint16_t* data, uint16_t address, uint8_t quantity)
{
	uint8_t buffer[kBufferSize];
	// build frame
	buffer[kSlaveAddressOffset] = slaveAddress;
	buffer[kFunctionCodeOffset] = kFunctionWriteRegisters;
	*(uint16_t*)(&buffer[kRegisterAddressOffset]) = asm_htons(address);
	*(uint16_t*)(&buffer[kRegisterQuantityOffset]) = asm_htons(quantity);
	for(uint8_t i = 0 ; i < quantity ; i++){
		*(uint16_t*)(&buffer[kWriteDataOffset + i * 2]) = asm_htons(data[i]);
	}	
	// send frame
	send(buffer, kWriteDataOffset + quantity * 2);
	// receive response
	uint8_t rxBytes;
	uint8_t error = receive(buffer, &rxBytes);
	if(error != kOK) return error; else return kOK;
}


void Slave::begin(uint8_t address)
{
	slaveAddress = address;
	format = 0;
}

int Slave::get(uint8_t address)
{
	uint16_t returnVal;
	if(getRegisters(&returnVal, address, 1) != kOK) return 0;
	return returnVal;
}


void Slave::set(uint8_t address, int value)
{
	uint16_t data[2];
	if((address == DisplayIntegerFormat) || (address == DisplayStringFormat)){
		format = value;
		return;
	} else if(address == DisplayInteger){
		data[0] = format;
		data[1] = value;
		setRegisters(data, DisplayIntegerFormat, 2);
	} else {
		data[0] = value;
		setRegisters(data, address, 1);
	}
}


void Slave::set(uint8_t address, char* string)
{
	const uint8_t kRegisters = 5;
	uint16_t registerValues[kRegisters];
	uint8_t i = 0;
	registerValues[i++] = format;
	
	while(i < kRegisters){
		registerValues[i] = *string;
		if(*string++ == 0){
			setRegisters(registerValues, DisplayStringFormat, i + 1);	
			//reg_setRegister(address, registerValue);
			break;
		}
		registerValues[i] |= (uint16_t)(*string) << 8;
		//reg_setRegister(address++, registerValue);
		if(*string++ == 0){
			setRegisters(registerValues, DisplayStringFormat, i + 1);
			break;
		}	
		i++;
	}
}


#define kLabel	0
#define kValue	1

void Slave::addMenu(MenuItem* menuList)
{
	menu = menuList;
	item = 0;
	state = kValue;
	lastState = kLabel;	// force an initial update of the display
	lastValue = 0;
	lastItem = 0;
	
	while(menu[items].label != NULL){
		items++;
	}	
}


void Slave::update(void)
{
	if(items == 0) return;
	
	// update state
	int key = get(Keypad);
	if(state == kValue){
		if(key == CenterShort) state = kLabel;
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
		if(key == CenterShort) state = kValue;	
		
	}
	
	// range check boolean
	if(menu[item].format == Bool){
		if(*menu[item].value > 1) *menu[item].value = 1;
		if(*menu[item].value < 0) *menu[item].value = 0;
	}
	
	// update display
	if(state == kValue){
		int newValue = *(menu[item].value);
		if((newValue != lastValue) || (lastState == kLabel)) {
			lastValue = newValue;
			lastState = kValue;
			if(menu[item].access == ReadWrite){
				set(DisplayIntegerFormat, menu[item].format | Flash);
			} else {
				set(DisplayIntegerFormat, menu[item].format);
			}
			set(DisplayInteger, newValue);
		}
	} else {
		if((lastState == kValue) || (lastItem != item)) {
			lastItem = item;
			lastState = kLabel;
			set(DisplayStringFormat, menu[item].format);
			set(DisplayString,  menu[item].label);
		}
	}
}

