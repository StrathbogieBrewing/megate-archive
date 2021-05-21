#ifndef TINDUINO_H
#define TINDUINO_H

#include "Arduino.h"
#include "tinframe.h"

#define TinDuino_kInterFrameMicros (12500L)
#define TinDuino_kBaud (1200)

enum {
  TinDuino_kOK = 0,
  TinDuino_kWriteBusy,
  TinDuino_kWriteCollision,
  TinDuino_kWriteTimeout,
  TinDuino_kWriteComplete,
  TinDuino_kReadNoData,
  TinDuino_kReadCRCError,
  TinDuino_kReadSequenceError,
  TinDuino_kReadOverunError,
};

class TinDuino {
public:
  TinDuino(HardwareSerial &serial, unsigned char interruptPin);
  void begin();
  int update();
  int write(tinframe_t* frame);
  int read(tinframe_t* frame);

private:
  HardwareSerial &serialPort;
  unsigned char rxInterruptPin;

  unsigned char sequence;
  tinframe_t txFrame;
  unsigned char txIndex;
  tinframe_t rxFrame;
  unsigned char rxIndex;

  static void externalInterrupt(void);
  static volatile unsigned long rxActiveMicros;
};

#endif //  TINDUINO_H
