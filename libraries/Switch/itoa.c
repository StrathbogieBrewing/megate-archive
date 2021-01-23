#include <string.h>

#include "itoa.h"

int _itoa(char* str, uint8_t format, int16_t value)
{
  unsigned char digit;
  uint8_t enable_digits = 0;
	char* ptr;

	ptr = str;

  // do sign
  if (value < 0){
      value = -value;
      *str++ = '-';
  }

  // do 10000's
 	digit = '0';
  while (value > 9999){
      value -= 10000;
      digit++;
  }
  if ((digit != '0') || (enable_digits != 0)) {
      *str++ = digit; 
      enable_digits = 1;
  }

  // do 1000's
  digit = '0';
  while(value > 999){
      value -= 1000;
      digit++;
  }
  if((digit != '0') || (enable_digits != 0)) {
      *str++ = digit;
      enable_digits = 1;
  }

  // do 100's
  digit = '0';
  while(value > 99){
      value -= 100;
      digit++;
  }
  if (format == Milli){
      if (enable_digits == 0) *str++ = '0';
      *str++ = '.';
      enable_digits = 1;
  }
  if((digit != '0') || (enable_digits != 0)) {
      *str++ = digit;
      enable_digits = 1;
  }

  // do 10's
  digit = '0';
  while(value > 9){
    value -= 10;
    digit++;
  }
  if (format == Centi){
    if (enable_digits == 0) *str++ = '0';
    *str++ = '.';
    enable_digits = 1;
  }
  if((digit != '0') || (enable_digits != 0)  || (format == TwoDigits) ) {
    *str++ = digit;
    enable_digits = 1;
  }

  // do 1's
  digit = '0' + value;
  if (format == Deci){
    if (enable_digits == 0) *str++ = '0';
    *str++ = '.';
  }
  *str++ = digit;
  *str = 0;

	return str - ptr;
}


/// Convert integer to ascii decimal string and send to terminal
/// @param integer value
/// @param desired ascii decimal format
uint8_t itoa_toString(char* str, int16_t value, uint8_t format)
{
	int16_t hours = 0;

	if(format == Time){
		while (value >= 60) {++hours ; value -= 60;}
		str += _itoa(str, TwoDigits, hours);
		*str++ = '.'; 
		_itoa(str, TwoDigits, value);
	} else if(format == Bool) {
		if(value) strcpy(str, "on"); else strcpy(str, "off");
	} else {
		_itoa(str, format, value);
	}
	return strlen(str);
}



