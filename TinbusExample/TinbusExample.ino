#include "Tinbus.h"

Tinbus tinbus(Serial2);

void setup() {
  tinbus.begin();
  Serial.begin(9600);
}

void loop() {
  delay(1000);
  tinbus.write("Hello");
}
