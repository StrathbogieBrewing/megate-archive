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
  int update();
  int write(tinbus_frame_t* frame);
  int read(tinbus_frame_t* frame);


private:
  HardwareSerial &serialPort;
  unsigned long serialBaud;
  unsigned char sequence;

  tinbus_frame_t txFrame;
  unsigned char txIndex;
  tinbus_frame_t rxFrame;
  unsigned char rxIndex;

  static void externalInterrupt(void);
  static volatile unsigned long rxActiveMicros;
};

#endif //  TINDUINO_H
