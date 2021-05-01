

#define kBlankPeriodus 5000

extern volatile unsigned long timer0_overflow_count;

int led = 5;        // the PWM pin the LED is attached to
int brightness = 0; // how bright the LED is
int fadeAmount = 1; // how many points to fade the LED by

volatile unsigned long deltaT = 0;

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);

  Serial.begin(9600);

  // ZCD interrupt init
  PCIFR |= (1 << PCIF0);
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0);
}

// the loop routine runs over and over again forever:
void loop() {
  // set the brightness of pin 9:
  analogWrite(led, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(2);

  unsigned int dt = 0;

  noInterrupts();
  dt = deltaT;
  deltaT = 0;
  interrupts();

  if (dt) {
    // Serial.println(dt);
    Serial.println((dt * 4444L) >> 8);
  }
}

ISR(PCINT0_vect) {
  static unsigned long time = 0;
  // unsigned long t = micros();
  unsigned long t = (timer0_overflow_count << 8) + TCNT0;

  // only respond to the falling edge of ZCD input
  if (0 == (PINB & (1 << PORTB0))) {
    deltaT = t - time;
    time = t;
  }
}
