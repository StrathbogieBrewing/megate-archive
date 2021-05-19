#ifndef TINDUINO_H
#define TINDUINO_H

#include "Arduino.h"
#include "tinbus.h"

#define TinDuino_kInterFrameMicros (12500L)
#define TinDuino_kBaud (1200)

class TinDuino {
public:
  TinDuino(HardwareSerial &serial, unsigned char interruptPin);
  void begin();
  int update();
  int write(tinbus_frame_t* frame);
  int read(tinbus_frame_t* frame);

private:
  HardwareSerial &serialPort;
  unsigned char rxInterruptPin;

  unsigned char sequence;
  tinbus_frame_t txFrame;
  unsigned char txIndex;
  tinbus_frame_t rxFrame;
  unsigned char rxIndex;

  static void externalInterrupt(void);
  static volatile unsigned long rxActiveMicros;
};

#endif //  TINDUINO_H
