#ifndef TINDUINO_H
#define TINDUINO_H

#include "Arduino.h"

#include "tinbus.h"

#define TinDuino_kInterFrameMicros (12500L)
#define TinDuino_kExternalInterruptPin (2)

class TinDuino {
public:
  TinDuino(HardwareSerial &serial, unsigned long baud = 1200);
  void begin();
  int write(tinbus_frame_t* txFrame);
  int read(tinbus_frame_t* rxData);

private:
  HardwareSerial &serialPort;
  unsigned long serialBaud;
  int sendByte(unsigned char byte);

  unsigned char sequence;
  static void externalInterrupt(void);
  static volatile unsigned long rxActiveMicros;
};

#endif //  TINDUINO_H
