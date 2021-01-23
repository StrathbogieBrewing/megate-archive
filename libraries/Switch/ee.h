#ifndef EE_H
#define EE_H

#ifdef __cplusplus
extern "C"{
#endif

#include "hal.h"

enum ee_reference {
	ee_SlaveAddress = 0,
	ee_eeprom1,
	ee_eeprom2,
	ee_eeprom3,
};


HAL* ee(void);

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* EE_H */
