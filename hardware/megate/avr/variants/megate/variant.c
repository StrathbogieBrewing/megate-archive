#include "Arduino.h"

#include <avr/sleep.h>

#define rtc_kSetTCCR2 ((1 << CS22) | (1 << CS20)) // overflow at 1 Hz (prescale 128)
#define rtc_kStayAwakeSeconds (10)

volatile unsigned char rtc_seconds;
volatile unsigned int rtc_minutes;
volatile unsigned char rtc_days;
volatile unsigned int rtc_stayAwakeTimer;
volatile unsigned int rtc_cal;

extern volatile unsigned long timer0_overflow_count;

ISR(TIMER2_OVF_vect) {
  static unsigned int old_t = 0;

  if (rtc_stayAwakeTimer){
    rtc_stayAwakeTimer--;
    // calibrate rc oscillator to 1 MHz if in stay awake
    unsigned int new_t = ((unsigned int)timer0_overflow_count << 8) + (unsigned int)TCNT0;
    rtc_cal = new_t - old_t;
    old_t = new_t;
    if(rtc_cal > 15700) OSCCAL--;
    if(rtc_cal < 15550) OSCCAL++;
  }

  // update real time clock
  if (++rtc_seconds >= 60) {
    rtc_seconds = 0;
    if (rtc_minutes >= 1440) {
      rtc_minutes = 0;
      rtc_days++;
    }
  }
}

ISR(INT0_vect) {
  rtc_stayAwakeTimer = rtc_kStayAwakeSeconds; // if uart rx pin active stay awake
}

void rtc_sleep(void){
  sleep_mode();      // Enter sleep mode.
  TCCR2 = kSetTCCR2; // Write dummy value to control register
  while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB)));
}

void initVariant() {
  rtc_seconds = 0;
  rtc_minutes = 60 * 10;  // start at 10 am from boot
  rtc_days = 0;
  rtc_stayAwakeTimer = rtc_kStayAwakeSeconds;

  ASSR |= (1 << AS2); // enable asynchronous clock
  TCNT2 = 0;          // reset counter / timer 2
  TCCR2 = rtc_kSetTCCR2;  // initialise prescaler for 1 Hz overflow
  while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) ;

  TIMSK |= (1 << TOIE2); // Set 8-bit Timer/Counter2 Overflow Interrupt Enable
  sei();
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_enable(); // Enabling sleep mode
}
