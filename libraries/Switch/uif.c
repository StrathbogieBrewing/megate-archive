#include <string.h>

#include "bios.h"
#include "itoa.h"
#include "uif.h"
#include "seg.h"
#include "asm.h"

enum uif_constants {
	uif_kSegmentDP = 1,
	uif_kNumberOfDigits = 4,
	uif_kSegmentBufferSize = 8,
};

static uint8_t format = 0;


static void init(uint8_t reference)
{
	bios_init();
}


static int16_t getValue(uint8_t reference)
{
	if(reference == uif_kKeypad){
		return bios_getKey();
	}
	return 0;
}


void uif_setDisplay(char* str)
{
	uint8_t segmentBuffer[uif_kSegmentBufferSize];
	uint8_t segmentBufferIndex = 0;
	uint8_t segments;

	memset(segmentBuffer, 0, uif_kSegmentBufferSize);
	while(*str){
		if(*str == '.'){
			if(segmentBufferIndex != 0){
				segmentBufferIndex--;
			}
			segments = segmentBuffer[segmentBufferIndex] | uif_kSegmentDP;
		} else {
			segments = seg_getSegments(*str);
		}
		segmentBuffer[segmentBufferIndex] = segments;
		if((format & Flash) == 0){
			segmentBuffer[segmentBufferIndex + uif_kNumberOfDigits] = segments;
		}
		str++;
		if((++segmentBufferIndex == 4) && (*str != '.')){
			break;
		}
	}
	memcpy(bios_segment, segmentBuffer, uif_kSegmentBufferSize);
}

static void setValue(uint8_t reference, int16_t value)
{
	static char buffer[uif_kSegmentBufferSize];
	static char* bufferPointer = 0;

	if(reference == uif_kDisplayFormat){
		format = value;
		return;
	}

	if(reference == uif_kDisplayInteger){
		itoa_toString(buffer, value, format & ~Flash);
		uif_setDisplay(buffer);
		return;
	}

	if(reference == uif_kDisplayString1){
		bufferPointer = buffer;
	}

	if(reference >= uif_kDisplayString1){
		*bufferPointer++ = asm_lo(value);
		if(asm_lo(value) == 0){
			uif_setDisplay(buffer);
		} else {
			*bufferPointer++ = asm_hi(value);
			if(asm_hi(value) == 0){
				uif_setDisplay(buffer);
			}
		}
		if(bufferPointer >= &buffer[uif_kSegmentBufferSize]){
			bufferPointer = buffer;
		}
		return;
	}
}


HAL uifHAL = {
	init,
	0,
	getValue,
	setValue
};


HAL* uif(void)
{
	return &uifHAL;
}
