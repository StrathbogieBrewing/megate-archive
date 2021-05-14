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

unsigned char Tinbus::crc8(const tinbus_frame_t *frame) {
  unsigned char crc = 0;
  unsigned const char *data = (unsigned char *)frame + 1; // no crc on start
  unsigned char bytes = Tinbus_kFrameSize - 2; // or on crc
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
  while (micros() - time < Tinbus_kInterFrameMicros) {
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

int Tinbus::write(tinbus_frame_t *txFrame) {
  // set start byte
  txFrame->start = 0;
  // set sequence number
  txFrame->sequence = ++sequence;
  // calculate and set crc
  unsigned char crc = crc8(txFrame);
  txFrame->crc = crc;

  // has less than interframe period elapsed since bus activity
  if (micros() - rxActiveMicros < Tinbus_kInterFrameMicros) {
    return Tinbus_kWriteBusy;
  }
  // is bus active now
  if (digitalRead(Tinbus_kExternalInterruptPin) == LOW) {
    return Tinbus_kWriteBusy;
  }

  // send frame
  unsigned char txCount = 0;
  while (txCount < Tinbus_kFrameSize) {
    unsigned char *data = (unsigned char *)txFrame;
    err = sendByte(*data++);
    if (err != Tinbus_kOK) {
      return err;
    }
  }
}

int Tinbus::read(tinbus_frame_t *rxFrame) {
  unsigned char rxCount = 0;
  unsigned char *frame = (unsigned char *)rxFrame;
  while (micros() - rxActiveMicros < Tinbus_kInterFrameMicros) {
    if (serialPort.available() > 0) {
      if (rxCount++ < Tinbus_kFrameSize) {
        *frame++ = serialPort.read();
      } else {

      }
      if(rxCount == Tinbus_kFrameSize){
        if(rxFrame->crc == crc8(rxFrame)){
          unsigned char expectedSequence = sequence + 1;
          sequence = rxFrame->sequence;
          if(sequence != expectedSequence){
            return Tinbus_kReadSequenceError;
          } else {
            return Tinbus_kOK;
          }
        } else {
          return Tinbus_kReadCRCError;
        }
      }

      // if (rxCount < Tinbus_kFrameSize) {
      //   rxBuffer[rxCount++] = data;
      // } else {
      //   // received 8 byte frame
      //   unsigned char crc = crc8(&rxBuffer[1]);
      //   if (crc == rxBuffer[7]) {
      //     memcpy(rxFrame, &rxBuffer[1], Tinbus_kDataSize);
      //     return Tinbus_kOK;
      //   } else {
      //     return Tinbus_kReadCRCError;
      //   }
      // }
    }
  }
  return Tinbus_kReadNoData;
}
