#include "Arduino.h"

#include <avr/sleep.h>
// #include <limits.h>

#include "rtc.h"

#define rtc_kSetTCCR2 ((1 << CS22) | (1 << CS20))

bool rtc_sleep(void) {
  GICR |= (1 << INT0);    // enable INT0, active low
  ADCSRA &= ~(1 << ADEN); // disable adc to save power
  // ACSR |= (1 << ACD);     // disable comparator
  unsigned char seconds = rtc_seconds;
  sleep_mode();         // Enter sleep mode.
  GICR &= ~(1 << INT0); // disable INT0
  ADCSRA |= (1 << ADEN);
  // TCCR2 = rtc_kSetTCCR2; // Write dummy value to control register
  // while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {
  // }

  // test if it was a timer wake up or external interrupt
  if (seconds == rtc_seconds)
    return true;
  else
    return false;
}

ISR(INT0_vect) {
  GICR &= ~(1 << INT0); // disable INT0
}

void initVariant() {
  rtc_init();
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_enable(); // Enabling sleep mode
}

// rtc_seconds = 0;
// rtc_minutes = 60 * 10; // start at 10 am from boot
// rtc_days = 0;
// secondsAwake = 0;
//
// ASSR |= (1 << AS2);    // enable asynchronous clock
// TCNT2 = 0;             // reset counter / timer 2
// TCCR2 = rtc_kSetTCCR2; // initialise prescaler for 1 Hz overflow
// while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {
// }
// TIMSK |= (1 << TOIE2); // Set 8-bit Timer/Counter2 Overflow Interrupt Enable
// sei();
//
// set_sleep_mode(SLEEP_MODE_PWR_SAVE);
// sleep_enable(); // Enabling sleep mode

// #define rtc_kTarget (122)
//
// #define kSecondsBeforeCalibration (2)
//
// // overflow at 1 Hz (prescale 128)
// #define rtc_kSetTCCR2 ((1 << CS22) | (1 << CS20))

// volatile unsigned char rtc_seconds;
// volatile unsigned int rtc_minutes;
// volatile unsigned char rtc_days;
// volatile unsigned char rtc_cal;

// static volatile unsigned char secondsToSleep = 0;
// static volatile unsigned char secondsAwake = 0;
// static volatile bool timerOverflowFlag = false;
//
// volatile unsigned char rtc_secondsSinceReceive = UCHAR_MAX;
// // static volatile bool receiveActiveFlag = false;
//
// extern volatile unsigned long timer0_overflow_count;

// ISR(TIMER2_OVF_vect) {
//   static unsigned char old_t = 0;
//
//   timerOverflowFlag = true;
//
//   // update real time clock
//   if (++rtc_seconds >= 60) {
//     rtc_seconds = 0;
//     if (++rtc_minutes >= 1440) {
//       rtc_minutes = 0;
//       rtc_days++;
//     }
//   }
//
//   if (secondsAwake < UCHAR_MAX)
//     secondsAwake++;
//
//   if (rtc_secondsSinceReceive < UCHAR_MAX)
//     rtc_secondsSinceReceive++;
//
//   // update rc calibration timer every 2 seconds
//   if (rtc_seconds & 0x01) {
//     unsigned char new_t = *((unsigned char *)(&timer0_overflow_count));
//     // more efficient way of doing (unsigned char)timer0_overflow_count;
//
//     // calibrate if device awake for more than kSecondsBeforeCalibration
//     if (secondsAwake > kSecondsBeforeCalibration) {
//       rtc_cal = new_t - old_t;
//       // calibrate rc oscillator to 1 MHz - target is 15625
//       if (rtc_cal > rtc_kTarget)
//         OSCCAL--;
//       if (rtc_cal < rtc_kTarget)
//         OSCCAL++;
//     }
//     old_t = new_t;
//   }
// }
