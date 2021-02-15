#ifndef RTC_H_
#define RTC_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define rtc_kTCCR2 ((1 << CS22) | (1 << CS20))

extern volatile unsigned char rtc_seconds;
extern volatile unsigned char rtc_minutes;
extern volatile unsigned char rtc_hours;
extern volatile unsigned char rtc_days;

// in rtc.S
// can block for upto 100 ms
void rtc_init(void);

// can block for upto 8 ms
int rtc_calibrate(void);

// in rtc.c
bool rtc_sleep(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RTC_H_ */
