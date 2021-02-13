#include <avr/sleep.h>

#include "rtc.h"

bool rtc_sleep(void) {

  ADCSRA &= ~(1 << ADEN);                 // disable adc to save power
  ACSR |= (1 << ACD);                     // disable comparator
  unsigned char seconds = rtc_seconds;
  sleep_mode();                           // Enter sleep mode.
  ADCSRA |= (1 << ADEN);

  // test if it was a timer wake up or external interrupt
  return (seconds != rtc_seconds);
}
