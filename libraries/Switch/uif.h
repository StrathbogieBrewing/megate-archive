#ifndef UIF_H
#define UIF_H

#include "hal.h"

#ifdef __cplusplus
extern "C"{
#endif

enum uif_reference {
	uif_kKeypad = 0,
	uif_kDisplayFormat,
	uif_kDisplayInteger,
	uif_kDisplayString1,
	uif_kDisplayString2,
	uif_kDisplayString3,
	uif_kDisplayString4
};

enum uif_format{
	NoFlash = 0,
	Flash = 128
};

HAL* uif(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* UIF_H */
