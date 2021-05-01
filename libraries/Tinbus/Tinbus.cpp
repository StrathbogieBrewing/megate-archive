

#include "Tinbus.h"

Tinbus::Tinbus(HardwareSerial &serial = Serial, unsigned long baud)
    : serialPort{serial}, serialBaud{baud} {}

void Tinbus::rxActive(void) { rxActiveMicros = micros(); }

void Tinbus::begin() {
  serialPort.begin(serialBaud);
}

unsigned char Tinbus::crc8(const unsigned char *data, int length) {
  unsigned char crc = 0x00;
  unsigned char extract;
  unsigned char sum;
  for (int i = 0; i < length; i++) {
    extract = *data;
    for (unsigned char tempI = 8; tempI; tempI--) {
      sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum)
        crc ^= 0x8C;
      extract >>= 1;
    }
    data++;
  }
  return crc;
}

// unsigned char Tinbus::crc8(const unsigned char data) {
//   for (unsigned char j = 0; j < 8; ++j) {
//     unsigned char mix = (crc ^ data) & 0x01;
//     crc >>= 1;
//     if (mix)
//       crc ^= 0xAB;
//     data >>= 1;
//   }
//   return crc;
// }

int Tinbus::sendByte(unsigned char byte) {
  serialPort.flush(); // wait for any previous tx data to be sent
  unsigned long time = micros();
  serialPort.write(byte);
  Serial.println(byte);
  while (micros() - time < 2000L) {
    if (serialPort.available() > 0) {
      if (serialPort.read() != byte) {
        return Tinbus_kWriteCollision;
      } else {
        return Tinbus_kOK;
      }
    }
  }
  return Tinbus_kWriteTimeout;
}

int Tinbus::write(const unsigned char *txData) {
  // has at least 2 ms elapsed since bus activity
  if (micros() - rxActiveMicros < 2000L) {
    return Tinbus_kWriteBusy;
  }
  // is bus currently active
  if (digitalRead(Tinbus_kExternalInterruptPin) == LOW) {
    return Tinbus_kWriteBusy;
  }
  // send initial zero
  int err = sendByte(0);
  if (err != Tinbus_kOK) {
    return err;
  }
  // send data
  unsigned char txCount = 0;

  while (txCount < Tinbus_kDataSize) {
    unsigned char data = txData[txCount++];
    // crc = crc_byte(data ^ crc);
    err = sendByte(data);
    if (err != Tinbus_kOK) {
      return err;
    }
  }
  // send crc
  unsigned char crc = crc8(txData[0], Tinbus_kDataSize);
  return sendByte(crc);
}

int Tinbus::read(unsigned char *rxData) {
  // if (micros() - rxActiveMicros > 2000L) {
  //   rxByteCount = 0;
  //   return Tinbus_kReadNoData;
  // }
  unsigned char rxBuffer[Tinbus_kFrameSize];
  unsigned char rxCount = 0;
  // unsigned char crc = 0;

  while (micros() - rxActiveMicros < 2000L) {
    if (serialPort.available() > 0) {
      unsigned char data = serialPort.read();
      if (rxCount < Tinbus_kFrameSize) {
        rxBuffer[rxCount++] = data;
        // crc = crc_byte(data ^ crc);
      } else {
        // received 8 byte frame
        unsigned char crc = crc8(&rxBuffer[1], Tinbus_kDataSize);
        if (crc == rxBuffer[7]) {
          memcpy(rxData, &rxBuffer[1], Tinbus_kDataSize);
          return Tinbus_kOK;
        } else {
          return Tinbus_kReadError;
        }
      }
    }
  }

  // rxByteCount = 0;
  return Tinbus_kReadNoData;
}
