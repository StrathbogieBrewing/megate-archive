#ifndef RTC_H_
#define RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

extern volatile unsigned char rtc_seconds;
extern volatile unsigned char rtc_minutes;
extern volatile unsigned char rtc_hours;
extern volatile unsigned char rtc_days;

void rtc_init(void);
int rtc_calibrate(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RTC_H_ */
