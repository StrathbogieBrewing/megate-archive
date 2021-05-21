#include "TinDuino.h"

#define kRXDataReady (tinframe_kFrameSize + 1)
#define kRXDone (tinframe_kFrameSize + 2)

#define kTXRequest (tinframe_kFrameSize + 1)
#define kTXIdle (tinframe_kFrameSize + 2)

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
      // immediately update rxActiveMicros
      noInterrupts();
      rxActiveMicros = micros();
      interrupts();
      return TinDuino_kWriteBusy;
    } else {
      txIndex = kTXIdle;
    }
  }

  if (txIndex < tinframe_kFrameSize) {
    unsigned char txData = ((char *)&txFrame)[txIndex];
    if (serialPort.available() > 0) {
      unsigned char rxData = serialPort.read();
      if (rxData == txData) {
        ++txIndex;
        if (txIndex < tinframe_kFrameSize) {
          txData = ((char *)&txFrame)[txIndex];
          serialPort.write(txData);
          return TinDuino_kWriteBusy;
        } else {
          txIndex = kTXIdle;
          return TinDuino_kWriteComplete;
        }
      } else {
        txIndex = kTXIdle;
        return TinDuino_kWriteCollision;
      }
    }
    return TinDuino_kWriteBusy;
  }

  if (serialPort.available() > 0) {
    unsigned char rxData = serialPort.read();
    if (rxIndex < tinframe_kFrameSize) {
      ((char *)&rxFrame)[rxIndex++] = rxData;
    } else {
      return TinDuino_kReadOverunError;
    }
    if (rxIndex == tinframe_kFrameSize) {
      if (tinframe_checkFrame(&rxFrame) == tinframe_kOK) {
      // if (rxFrame.crc == tinframe_crcFrame(&rxFrame)) {
        // unsigned char expectedSequence = sequence + 1;
        // sequence = rxFrame.sequence;
        rxIndex = kRXDataReady;
        // if (sequence == expectedSequence) {
        return TinDuino_kOK;
        // } else {
        //   return TinDuino_kReadSequenceError;
        // }
      } else {
        rxIndex = kRXDone;
        return TinDuino_kReadCRCError;
      }
    }
  }
  return TinDuino_kReadNoData;
}

int TinDuino::write(tinframe_t *frame) {
  if (txIndex != kTXIdle) {
    return TinDuino_kWriteBusy;
  }
  tinframe_prepareFrame(frame);
  // frame->start = tinframe_kStart;                // set start byte
  // // frame->sequence = ++sequence;             // set sequence number
  // unsigned char crc = tinframe_crcFrame(frame);  // calculate crc
  // frame->crc = crc;                            // set crc
  memcpy(&txFrame, frame, tinframe_kFrameSize);
  txIndex = kTXRequest;
  return TinDuino_kOK;
}

int TinDuino::read(tinframe_t *frame) {
  if (rxIndex == kRXDataReady) {
    memcpy(frame, &rxFrame, tinframe_kFrameSize);
    rxIndex = kRXDone;
    return TinDuino_kOK;
  }
  return TinDuino_kReadNoData;
}
