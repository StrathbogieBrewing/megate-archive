#include "Tinbus.h"

Tinbus::Tinbus(HardwareSerial &serial = Serial, unsigned long baud)
    : serialPort{serial}, serialBaud{baud} {}

static volatile unsigned long Tinbus::rxActiveMicros = 0;

static void Tinbus::externalInterrupt(void) { rxActiveMicros = micros(); }

void Tinbus::begin() {
  serialPort.begin(serialBaud);
  pinMode(Tinbus_kExternalInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(Tinbus_kExternalInterruptPin),
                  externalInterrupt, CHANGE);
}

// CRC-8 uses DVB-S2 polynomial
unsigned char Tinbus::crc8(unsigned char crc, unsigned char data) {
  unsigned char i;
  for (i = 0x80; i > 0; i >>= 1) {
    bool bit = crc & 0x80;
    if (data & i)
      bit = !bit;
    crc <<= 1;
    if (bit)
      crc ^= 0xd5;
  }
  return crc;
}

unsigned char Tinbus::crc8(const unsigned char *data) {
  unsigned char crc = 0;
  unsigned char bytes = Tinbus_kDataSize;
  while (bytes--) {
    crc = crc8(crc, *data++);
  }
  return crc;
}

int Tinbus::sendByte(unsigned char byte) {
  serialPort.flush(); // wait for any previous tx data to be sent
  unsigned long time = micros();
  serialPort.write(byte);
  // Serial.println(byte, HEX);
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
  // has less than 2 ms elapsed since bus activity
  if (micros() - rxActiveMicros < 2000L) {
    return Tinbus_kWriteBusy;
  }
  // is bus active now
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
    err = sendByte(data);
    if (err != Tinbus_kOK) {
      return err;
    }
  }
  // send crc
  unsigned char crc = crc8(txData);
  return sendByte(crc);
}

int Tinbus::read(unsigned char *rxData) {
  unsigned char rxBuffer[Tinbus_kFrameSize];
  unsigned char rxCount = 0;
  while (micros() - rxActiveMicros < 2000L) {
    if (serialPort.available() > 0) {
      unsigned char data = serialPort.read();
      if (rxCount < Tinbus_kFrameSize) {
        rxBuffer[rxCount++] = data;
      } else {
        // received 8 byte frame
        unsigned char crc = crc8(&rxBuffer[1]);
        if (crc == rxBuffer[7]) {
          memcpy(rxData, &rxBuffer[1], Tinbus_kDataSize);
          return Tinbus_kOK;
        } else {
          return Tinbus_kReadCRCError;
        }
      }
    }
  }
  return Tinbus_kReadNoData;
}
