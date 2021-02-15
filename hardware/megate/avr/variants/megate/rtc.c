#include <avr/sleep.h>

#include "rtc.h"

bool rtc_sleep(void) {

  ADCSRA &= ~(1 << ADEN);                 // disable adc to save power
  unsigned char seconds = rtc_seconds;
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_mode();                           // Enter sleep mode.
  ADCSRA |= (1 << ADEN);
  
  TCCR2 = ((1 << CS22) | (1 << CS20)); // dummy write
  while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {
  }

  // test if it was a timer wake up or external interrupt
  return (seconds != rtc_seconds);
}
