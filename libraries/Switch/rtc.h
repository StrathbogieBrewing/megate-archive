#ifndef RTC_H_
#define RTC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "hal.h"

#define rtc_kMinTime (0)
#define rtc_kMaxTime ((60*24)-1)

HAL* rtc(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // RTC_H_
