#include "Arduino.h"

#include <avr/sleep.h>

#include "cal.h"

#define rtc_kTarget (122)
#define rtc_kStayAwakeSeconds (10)

// overflow at 1 Hz (prescale 128)
#define rtc_kSetTCCR2 ((1 << CS22) | (1 << CS20))

volatile unsigned char rtc_seconds;
volatile unsigned int rtc_minutes;
volatile unsigned char rtc_days;
volatile unsigned char rtc_awakeTimer;
volatile unsigned char rtc_cal;

// static volatile bool timer2overflow;
static volatile bool rtc_sleeping = false;

extern volatile unsigned long timer0_overflow_count;

ISR(TIMER2_OVF_vect) {
  static unsigned char old_t = 0;

  // update real time clock
  if (++rtc_seconds >= 60) {
    rtc_seconds = 0;
    if (++rtc_minutes >= 1440) {
      rtc_minutes = 0;
      rtc_days++;
    }
  }

  // update awake timer
  if (rtc_awakeTimer < rtc_kStayAwakeSeconds)
    rtc_awakeTimer++;

  // update rc calibration timer every 2 seconds
  if (rtc_seconds & 0x01) {
    unsigned char new_t = *((unsigned char*)(&timer0_overflow_count));
    // (unsigned char)timer0_overflow_count;

    // calibrate if device awake for more than 2 seconds
    if (rtc_awakeTimer > 2) {
      rtc_cal = new_t - old_t;
      // calibrate rc oscillator to 1 MHz - target is 15625
      if (rtc_cal > rtc_kTarget)
        OSCCAL--;
      if (rtc_cal < rtc_kTarget)
        OSCCAL++;
    }
    old_t = new_t;
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
