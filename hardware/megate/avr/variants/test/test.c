#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>



int main(void){
  ASSR |= (1 << AS2);    // enable asynchronous clock
  TCNT2 = 0;             // reset counter / timer 2
  TCCR2 = ((1 << CS22) | (1 << CS20)); // initialise prescaler for 1 Hz overflow
  while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {
  }
  TIMSK |= (1 << TOIE2); // Set 8-bit Timer/Counter2 Overflow Interrupt Enable
  sei();

  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_enable(); // Enabling sleep mode

  DDRD |= (1 << PD7); // || (1 << PD1);

  // PORTD |= (1 << PD1);

  // ACSR |= (1 << ACD);

  while (1){
    sleep_mode();          // Enter sleep mode.
    PORTD ^= (1 << PD7);

  }
}

ISR(TIMER2_OVF_vect) {

}
