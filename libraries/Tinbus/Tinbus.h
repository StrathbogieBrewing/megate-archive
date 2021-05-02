#ifndef TINBUS_H
#define TINBUS_H

#include "Arduino.h"

#define Tinbus_kFrameSize (8)
#define Tinbus_kDataSize (6)

#define Tinbus_kExternalInterruptPin (2)

#define Tinbus_kOK (0)
#define Tinbus_kWriteBusy (-1)
#define Tinbus_kWriteCollision (-2)
#define Tinbus_kWriteTimeout (-3)
#define Tinbus_kReadNoData (-4)
#define Tinbus_kReadCRCError (-5)

class Tinbus {
public:
  Tinbus(HardwareSerial &serial, unsigned long baud = 9600);
  void begin();
  static void externalInterrupt(void);
  unsigned char crc8(const unsigned char *data);
  unsigned char crc8(unsigned char crc, unsigned char data);
  int sendByte(unsigned char byte);
  int write(const unsigned char* txData);
  int read(unsigned char* rxData);
  static volatile unsigned long rxActiveMicros;
private:
  HardwareSerial &serialPort;
  unsigned long serialBaud;
};

#endif //  TINBUS_H
