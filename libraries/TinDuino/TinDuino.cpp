#include "TinDuino.h"

#define kRXDataReady (tinbus_kFrameSize + 1)
#define kRXDone (tinbus_kFrameSize + 2)

#define kTXRequest (tinbus_kFrameSize + 1)
#define kTXIdle (tinbus_kFrameSize + 2)


TinDuino::TinDuino(HardwareSerial &serial, unsigned char interruptPin)
    : serialPort{serial}, rxInterruptPin{interruptPin} {}

volatile unsigned long TinDuino::rxActiveMicros = 0;

void TinDuino::externalInterrupt(void) { rxActiveMicros = micros(); }

void TinDuino::begin() {
  serialPort.begin(TinDuino_kBaud);
  pinMode(rxInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(rxInterruptPin),
                  externalInterrupt, CHANGE);
  rxIndex = kRXDone;
  txIndex = kTXIdle;
}

int TinDuino::update() {
  noInterrupts();
  unsigned long lastActivity = micros() - rxActiveMicros;
  interrupts();
  if (lastActivity > TinDuino_kInterFrameMicros) {
    rxIndex = 0;
    while (serialPort.available() > 0) {
      serialPort.read();
    }
    if (txIndex == kTXRequest) {
      txIndex = 0;
      unsigned char txData = ((char *)&txFrame)[txIndex];
      serialPort.write(txData);
      // imediately update rxActiveMicros
      noInterrupts();
      rxActiveMicros = micros();
      interrupts();
      return tinbus_kWriteBusy;
    } else {
      txIndex = kTXIdle;
    }
  }

  if (txIndex < tinbus_kFrameSize) {
    unsigned char txData = ((char *)&txFrame)[txIndex];
    if (serialPort.available() > 0) {
      unsigned char rxData = serialPort.read();
      if (rxData == txData) {
        ++txIndex;
        if (txIndex < tinbus_kFrameSize) {
          txData = ((char *)&txFrame)[txIndex];
          serialPort.write(txData);
          return tinbus_kWriteBusy;
        } else {
          txIndex = kTXIdle;
          return tinbus_kWriteComplete;
        }
      } else {
        txIndex = kTXIdle;
        return tinbus_kWriteCollision;
      }
    }
    return tinbus_kWriteBusy;
  }

  if (serialPort.available() > 0) {
    unsigned char rxData = serialPort.read();
    if (rxIndex < tinbus_kFrameSize) {
      ((char *)&rxFrame)[rxIndex++] = rxData;
    } else {
      return tinbus_kReadOverunError;
    }
    if (rxIndex == tinbus_kFrameSize) {
      if (rxFrame.crc == tinbus_crcFrame(&rxFrame)) {
        unsigned char expectedSequence = sequence + 1;
        sequence = rxFrame.sequence;
        rxIndex = kRXDataReady;
        if (sequence == expectedSequence) {
          return tinbus_kOK;
        } else {
          return tinbus_kReadSequenceError;
        }
      } else {
        rxIndex = kRXDone;
        return tinbus_kReadCRCError;
      }
    }
  }
  return tinbus_kReadNoData;
}

int TinDuino::write(tinbus_frame_t *frame) {
  if (txIndex != kTXIdle) {
    return tinbus_kWriteBusy;
  }

  frame->start = 0;
  // set sequence number
  frame->sequence = ++sequence;
  // calculate and set crc
  unsigned char crc = tinbus_crcFrame(frame);
  frame->crc = crc;

  memcpy(&txFrame, frame, tinbus_kFrameSize);
  txIndex = kTXRequest;
  return tinbus_kOK;
}

int TinDuino::read(tinbus_frame_t *frame) {
  if (rxIndex == kRXDataReady) {
    memcpy(frame, &rxFrame, tinbus_kFrameSize);
    rxIndex = kRXDone;
    return tinbus_kOK;
  }
  return tinbus_kReadNoData;
}
