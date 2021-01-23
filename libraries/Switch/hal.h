#ifndef HAL_H_
#define HAL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef void (*initFunction)(uint8_t reference);
typedef void (*updateFunction)(uint8_t reference);
typedef int16_t (*getValueFunction)(uint8_t reference);
typedef void (*setValueFunction)(uint8_t reference, int16_t value);

typedef const struct{
	initFunction		init;
	updateFunction		update;
	getValueFunction 	getValue;
	setValueFunction 	setValue;
} HAL;

// returns the value set
typedef HAL* (*getHAL)(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* DEVICE_H_ */
