#include "Tinbus.h"

Tinbus tinbus(Serial2);

void extInt(void){
  tinbus.rxActive();
}

void setup() {
  // put your setup code here, to run once:
  tinbus.begin();

  pinMode(Tinbus_kExternalInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(Tinbus_kExternalInterruptPin),
                  extInt, CHANGE);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  // Serial.println(
    tinbus.write("Hello");

}
