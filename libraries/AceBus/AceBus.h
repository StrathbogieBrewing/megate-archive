#ifndef ACEBUS_H
#define ACEBUS_H

#include "Arduino.h"
#include "tinframe.h"

#define AceBus_kInterFrameMicros (12500L)
#define AceBus_kBaud (1200)

enum {
  AceBus_kOK = 0,
  AceBus_kWriteBusy,
  AceBus_kWriteCollision,
  AceBus_kWriteTimeout,
  AceBus_kWriteComplete,
  AceBus_kReadNoData,
  AceBus_kReadCRCError,
  AceBus_kReadSequenceError,
  AceBus_kReadOverunError,
  AceBus_kReadDataReady
};

class AceBus {
public:
  AceBus(HardwareSerial &serial, unsigned char interruptPin);
  void begin();
  int update();
  int write(tinframe_t* frame);
  int read(tinframe_t* frame);

private:
  HardwareSerial &serialPort;
  unsigned char rxInterruptPin;

  // unsigned char sequence;
  tinframe_t txFrame;
  unsigned char txIndex;
  tinframe_t rxFrame;
  unsigned char rxIndex;

  static void externalInterrupt(void);
  static volatile unsigned long rxActiveMicros;
};

#endif //  ACEBUS_H
