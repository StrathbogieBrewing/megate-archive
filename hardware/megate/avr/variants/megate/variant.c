#include "Arduino.h"

#include <avr/sleep.h>

#include "cal.h"

#define rtc_kStayAwakeSeconds (10)

// overflow at 1 Hz (prescale 128)
#define rtc_kSetTCCR2  ((1 << CS22) | (1 << CS20))

volatile unsigned char rtc_seconds;
volatile unsigned int rtc_minutes;
volatile unsigned char rtc_days;
volatile unsigned char rtc_awakeTimer;
volatile unsigned int rtc_cal;

// static volatile bool timer2overflow;
static volatile bool rtc_sleeping = false;

extern volatile unsigned long timer0_overflow_count;

ISR(TIMER2_OVF_vect) {
  static unsigned int old_t = 0;

  // update rc calibration timer
  unsigned int new_t =
        ((unsigned int)timer0_overflow_count << 8) + (unsigned int)TCNT0;
  // update awake timer
  if(rtc_awakeTimer < rtc_kStayAwakeSeconds)
    rtc_awakeTimer++;
  // calibrate if device awake more than 1 second
  if (rtc_awakeTimer > 1) {
    rtc_cal = new_t - old_t;
    // calibrate rc oscillator to 1 MHz
    if (rtc_cal > 15700)
      OSCCAL--;
    if (rtc_cal < 15550)
      OSCCAL++;
  }
  old_t = new_t;

  // update real time clock
  if (++rtc_seconds >= 60) {
    rtc_seconds = 0;
    if (++rtc_minutes >= 1440) {
      rtc_minutes = 0;
      rtc_days++;
    }
  }
}

ISR(INT0_vect) {
  // external interrupt 0 is tied to uart rx pin
  // if uart rx pin is active then trigger wake up
  // the pin may wake device without executing this interrupt
  // so consistently to do nothing
}

bool rtc_sleep(void) {

      GICR |= (1 << INT0); // enable INT0, active low
      // timer2overflow = false;
      sleep_mode();          // Enter sleep mode.
      GICR &= ~(1 << INT0);  // disable INT0
      TCCR2 = rtc_kSetTCCR2; // Write dummy value to control register
      while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB)))
        ;

      rtc_awakeTimer = 0;
      // if rx signal triggered wake up reset the stay awake timer
      // if (timer2overflow == false)
      //   rtc_stayAwakeTimer = rtc_kStayAwakeSeconds;
      //
      return true;

    // return false;
  }
//   if (rtc_stayAwakeTimer == 0) {
//     GICR |= (1 << INT0); // enable INT0, active low
//     timer2overflow = false;
//     sleep_mode();          // Enter sleep mode.
//     GICR &= ~(1 << INT0);  // disable INT0
//     TCCR2 = rtc_kSetTCCR2; // Write dummy value to control register
//     while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB)))
//       ;
//     // if rx signal triggered wake up reset the stay awake timer
//     if (timer2overflow == false)
//       rtc_stayAwakeTimer = rtc_kStayAwakeSeconds;
//
//     return true;
//   }
//   return false;
// }

void initVariant() {
  // fast rc calibration routine
  cal_init();

  rtc_seconds = 0;
  rtc_minutes = 60 * 10; // start at 10 am from boot
  rtc_days = 0;
  rtc_awakeTimer = 0;

  ASSR |= (1 << AS2);    // enable asynchronous clock
  TCNT2 = 0;             // reset counter / timer 2
  TCCR2 = rtc_kSetTCCR2; // initialise prescaler for 1 Hz overflow
  while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {
  }
  TIMSK |= (1 << TOIE2); // Set 8-bit Timer/Counter2 Overflow Interrupt Enable
  sei();

  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_enable(); // Enabling sleep mode
}
