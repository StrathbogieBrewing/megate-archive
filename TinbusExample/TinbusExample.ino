#include "Tinbus.h"

Tinbus tinbus(Serial);

void setup() {
  tinbus.begin();
  // Serial.begin(9600);
}

void loop() {

  tinbus.write("Hello");
  delay(1000);
  tinbus.write("Bye..");
  delay(1000);
}
