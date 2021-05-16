#include "TinDuino.h"

TinDuino::TinDuino(HardwareSerial &serial = Serial, unsigned long baud)
    : serialPort{serial}, serialBaud{baud} {}

volatile unsigned long TinDuino::rxActiveMicros = 0;

void TinDuino::externalInterrupt(void) { rxActiveMicros = micros(); }

void TinDuino::begin() {
  serialPort.begin(serialBaud);
  pinMode(TinDuino_kExternalInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(TinDuino_kExternalInterruptPin),
                  externalInterrupt, CHANGE);
}

int TinDuino::sendByte(unsigned char byte) {
  serialPort.flush(); // wait for any previous tx data to be sent
  unsigned long time = micros();
  serialPort.write(byte);
  // Serial.println(byte, HEX);
  while (micros() - time < TinDuino_kInterFrameMicros) {
    if (serialPort.available() > 0) {
      if (serialPort.read() != byte) {
        return tinbus_kWriteCollision;
      } else {
        return tinbus_kOK;
      }
    }
  }
  return tinbus_kWriteTimeout;
}

int TinDuino::write(tinbus_frame_t *txFrame) {
  // set start byte
  txFrame->start = 0;
  // set sequence number
  txFrame->sequence = ++sequence;
  // calculate and set crc
  unsigned char crc = tinbus_crcFrame(txFrame);
  txFrame->crc = crc;

  // has less than interframe period elapsed since bus activity
  if (micros() - rxActiveMicros < TinDuino_kInterFrameMicros) {
    return tinbus_kWriteBusy;
  }
  // is bus active now
  if (digitalRead(TinDuino_kExternalInterruptPin) == LOW) {
    return tinbus_kWriteBusy;
  }

  // send frame
  unsigned char txCount = 0;
  unsigned char *data = (unsigned char *)txFrame;
  while (txCount++ < tinbus_kFrameSize) {
    int err = sendByte(*data++);
    if (err != tinbus_kOK) {
      return err;
    }
  }
  return tinbus_kOK;
}

int TinDuino::read(tinbus_frame_t *rxFrame) {
  unsigned char rxCount = 0;
  unsigned char *frame = (unsigned char *)rxFrame;
  while (micros() - rxActiveMicros < TinDuino_kInterFrameMicros) {
    if (serialPort.available() > 0) {
      if (rxCount++ < tinbus_kFrameSize) {
        *frame++ = serialPort.read();
      } else {
        return tinbus_kReadOverunError;
      }
      if(rxCount == tinbus_kFrameSize){
        if(rxFrame->crc == tinbus_crcFrame(rxFrame)){
          unsigned char expectedSequence = sequence + 1;
          sequence = rxFrame->sequence;
          if(sequence != expectedSequence){
            return tinbus_kReadSequenceError;
          } else {
            return tinbus_kOK;
          }
        } else {
          return tinbus_kReadCRCError;
        }
      }
    }
  }
  return tinbus_kReadNoData;
}
