#ifndef VARIANT_H_
#define VARIANT_H_

#ifdef __cplusplus
extern "C" {
#endif

extern volatile unsigned char rtc_seconds;
extern volatile unsigned int rtc_minutes;
extern volatile unsigned char rtc_days;
extern volatile unsigned char rtc_cal;
extern volatile unsigned char rtc_secondsSinceReceive;

bool rtc_sleep(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* VARIANT_H_ */
