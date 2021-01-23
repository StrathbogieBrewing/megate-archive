#ifndef HONEYWELLNTC_H_
#define HONEYWELLNTC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "hal.h"

HAL* ntcHoneywell(void);

int16_t ntcHoneywell_getDeciCelcius(int16_t rawADC);

#ifdef __cplusplus
} // extern "C"
#endif


#endif // #ifndef HONEYWELLNTC_H_
