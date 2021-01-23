#include <avr/wdt.h>

#include "switch.h"

// declare global variables
int16_t temperature;
int16_t pressure;
int16_t count;
int16_t pumpStatus = 0;
int16_t setPoint = 200;
int16_t hysteresis = 10;

// declare menus
MenuItem menuItems[] = {
  {"Temp", Deci, ReadOnly, &temperature},
  {"pres", Centi, ReadOnly, &pressure},
  {"cnt", Unit, ReadOnly, &count},
  {"Pump", Bool, ReadWrite, &pumpStatus},
  {"Set", Deci, ReadWrite, &setPoint},
  {"Hyst", Deci, ReadWrite, &hysteresis},
  {0}   // must end with a zero to indicate end of list
};

void setup() 
{
    wdt_enable(WDTO_2S);
    Switch.begin();
    Switch.addMenu(menuItems);
}

void loop() {
  // update measurements
  temperature = Switch.get(Philips1);
  pressure = Switch.get(InputI2C);
  count = Switch.get(Counter2);
  
  // update logic
  if(temperature > setPoint + hysteresis) Switch.set(Output1, pwm_On);
  if(temperature < setPoint - hysteresis) Switch.set(Output1, pwm_Off);

  // update switch
  Switch.update();
  
  // kick the dog
  wdt_reset();
  
}
