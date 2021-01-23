#ifndef ADC_H_
#define ADC_H_

#include "hal.h"

#ifdef __cplusplus
extern "C"{
#endif

enum adc_reference{
	adc_kInput1 = 1,
	adc_kInput2,
	adc_kCounter1,
	adc_kCounter2,
	adc_kFrequency1,
	adc_kFrequency2,

};

HAL* adc(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* ADC_H_ */
