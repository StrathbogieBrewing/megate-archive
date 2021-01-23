#include <avr/wdt.h>

#include "slave.h"

// declare slaves
const int NUMBEROFSLAVES = 6;
Slave slave[NUMBEROFSLAVES + 1];

// declare global variables
int16_t temperature;
int16_t pumpStatus = 0;
int16_t setPoint = 200;
int16_t hysteresis = 10;

// declare menus
MenuItem menuOne[] = {
  {"Temp", Deci, ReadOnly, &temperature},
  {"Pump", Bool, ReadWrite, &pumpStatus},
  {"Set", Deci, ReadWrite, &setPoint},
  {"Hyst", Deci, ReadWrite, &hysteresis},
  {0} // must end with a zero to indicate end of list
};

MenuItem menuTwo[] = {
  {"Temp", Deci, ReadOnly, &temperature},
  {0} // must end with a zero to indicate end of list
};

MenuItem menuThree[] = {
  {"Temp", Deci, ReadOnly, &temperature},
  {0} // must end with a zero to indicate end of list
};

MenuItem menuFour[] = {
  {"Temp", Deci, ReadOnly, &temperature},
  {0} // must end with a zero to indicate end of list
};

MenuItem menuFive[] = {
  {"Temp", Deci, ReadOnly, &temperature},
  {0} // must end with a zero to indicate end of list
};

MenuItem menuSix[] = {
  {"Temp", Deci, ReadOnly, &temperature},
  {0} // must end with a zero to indicate end of list
};

MenuItem* menus[] = {0, menuOne, menuTwo, menuThree, menuFour, menuFive, menuSix};

void setup() {
  wdt_enable(WDTO_2S);
  // initialise slaves and menus
  for(int i = 1 ; i <= NUMBEROFSLAVES ; i++){
    slave[i].begin(i);
    slave[i].addMenu(menus[i]);
  }
}

void loop() {
  // update measurements
  temperature = slave[1].get(Philips1);
  
  // update logic
  if(temperature > setPoint + hysteresis) slave[1].set(Output1, pwm_On);
  if(temperature < setPoint - hysteresis) slave[1].set(Output1, pwm_Off);

  // update menus
  for(int i = 1 ; i <= NUMBEROFSLAVES ; i++){
    slave[i].update();
  }
  
  // kick the dog
  wdt_reset();
  
  delay(100);
}
