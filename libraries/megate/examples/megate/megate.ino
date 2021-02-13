#include "AceMenu.h"
#include "Terminal.h"

#include "rtc.h"

#define kTemperatureDelta 3
#define kEepromStartTime ((uint16_t *)0)
#define kEepromDuration ((uint16_t *)2)
#define kEepromEvents ((uint16_t *)4)

int temperature = 200;
int heater = 0;

const int pwm_pin = 9; // ic pin 15

// getter functions
int getTime() { return rtc_minutes; }
int getOsc() { return (unsigned int)OSCCAL; }
int getCal() { return rtc_calibrate(); }
int getStartTime() { return (int)eeprom_read_word(kEepromStartTime); }
int getDuration() { return (int)eeprom_read_word(kEepromDuration); }
int getEvents() { return (int)eeprom_read_word(kEepromEvents); }
int getSeconds() { return rtc_seconds; }

// setters functions
void setTime(int v) { rtc_minutes = v; }
void setStartTime(int v) { eeprom_write_word(kEepromStartTime, (uint16_t)v); }
void setDuration(int v) { eeprom_write_word(kEepromDuration, (uint16_t)v); }
void setEvents(int v) { eeprom_write_word(kEepromEvents, (uint16_t)v); }

// menu contents
menuItem menuItems[] = {
    // name, getter, setter, format, min value , max value
    {"CAL", getCal, NULL, Format::unit, 0, 1439},
    {"CLOCK", getTime, setTime, Format::time, 0, 1439},
    {"SECS", getSeconds, NULL, Format::unit, 0, 1},
    {"START", getStartTime, setStartTime, Format::time, 0, 1439},
    {"DURATION", getDuration, setDuration, Format::unit, 1, 59},
    {"EVENTS", getEvents, setEvents, Format::unit, 1, 24},
    {"OSC", getOsc, NULL, Format::unit, 0, 1},
};

#define kMenuCount (sizeof(menuItems) / sizeof(menuItem))

Terminal terminal(Serial, 9600);

AceMenu menu = AceMenu(menuItems, kMenuCount, terminal, terminal);

void setup() {
  // initialise the menu
  menu.begin();

  // pinMode(pwm_pin, OUTPUT);
  // analogWrite(pwm_pin, 128);
  // OSCCAL = 190;
  // Serial.begin(9600);

  DDRD |= (1 << PD7);
}

void loop() {

  // printf("cal=%d\tosc=%d\tsec=%d\n", rtc_cal, (unsigned int)OSCCAL,
  // rtc_seconds);

  // static unsigned char seconds = rtc_seconds;

  PORTD ^= (1 << PD7);
  int cal = rtc_calibrate();
  PORTD ^= (1 << PD7);

  Serial.println(rtc_minutes);
  Serial.println(rtc_seconds);
  Serial.println(cal);
  Serial.println(OSCCAL);
  Serial.flush();

  // while(seconds == rtc_seconds);
  // seconds = rtc_seconds;

  // delay(1000);



  if (rtc_minutes & 0x01)
  {  GICR |= (1 << INT0);    // enable INT0, active low
    rtc_sleep();
    GICR &= ~(1 << INT0); // disable INT0
  }
  else {
    unsigned char s = rtc_seconds;
    while (s == rtc_seconds) {
    }
  }

  // if(rtc_sleep() == false){
  // update the menu if not in sleep state
  // menu.update();
  // };      // Enter sleep mode.

  // update temperature control
  // if (heater == true) {
  //   if (temperature > getStartTime() + getDuration())
  //     heater = false;
  // } else {
  //   if (temperature < getStartTime() - getDuration())
  //     heater = true;
  // }

  // for demo make the temperature value change every 500 ms
  // static int temperatureDelta = kTemperatureDelta;
  // static long unsigned int waitUntil = 0;
  // if (millis() > waitUntil) {
  //   waitUntil = millis() + 500L;
  //   // ramp up and down
  //   temperature += temperatureDelta;
  //   if (temperature > 250)
  //     temperatureDelta = -kTemperatureDelta;
  //   if (temperature < 150)
  //     temperatureDelta = kTemperatureDelta;
  // }
}

ISR(INT0_vect) {
  GICR &= ~(1 << INT0); // disable INT0
}
