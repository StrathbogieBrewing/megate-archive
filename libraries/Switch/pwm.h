#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C"{
#endif

#include "hal.h"

enum pwm_reference{
	pwm_kOutput1 = 1,
	pwm_kOutput2
};

enum pwm_constants{
	pwm_Off = 0,
	pwm_On = 255
};

HAL* pwm(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* PWM_H */
