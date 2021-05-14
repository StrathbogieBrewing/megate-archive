#ifndef TINBUS_H
#define TINBUS_H

#include "Arduino.h"

#define Tinbus_kDataSize (8)

typedef struct {
  const unsigned char start;
  const unsigned char msgID;
  const unsigned char sequence;
  const unsigned char data[Tinbus_kDataSize];
  const unsigned char crc;
} tinbus_frame_t;

#define Tinbus_kFrameSize (sizof(tinbus_frame_t))

// interframe period set to 1.5 characters (12500 us @ 1200 baud)
#define Tinbus_kInterFrameMicros (12500L)
#define Tinbus_kExternalInterruptPin (2)

enum {
  Tinbus_kOK = 0,
  Tinbus_kWriteBusy,
  Tinbus_kWriteCollision,
  Tinbus_kWriteTimeout,
  Tinbus_kReadNoData,
  Tinbus_kReadCRCError,
  Tinbus_kReadSequenceError,
  Tinbus_kReadOverunError,
}

// #define Tinbus_kOK (0)
// #define Tinbus_kWriteBusy (-1)
// #define Tinbus_kWriteCollision (-2)
// #define Tinbus_kWriteTimeout (-3)
// #define Tinbus_kReadNoData (-4)
// #define Tinbus_kReadCRCError (-5)

class Tinbus {
public:
  Tinbus(HardwareSerial &serial, unsigned long baud = 1200);
  void begin();
  int write(const tinbus_frame_t* txFrame);
  int read(tinbus_frame_t* rxData);

private:
  HardwareSerial &serialPort;
  unsigned long serialBaud;
  int sendByte(unsigned char byte);
  unsigned char crc8(const unsigned char *data);
  unsigned char crc8(unsigned char crc, unsigned char data);
  static void externalInterrupt(void);
  static volatile unsigned long rxActiveMicros;
  static volatile unsigned char sequence;
};

#endif //  TINBUS_H
