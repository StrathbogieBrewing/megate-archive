#include "TinDuino.h"

#define kRXDataReady (tinbus_kFrameSize + 1)
#define kRXDone (tinbus_kFrameSize + 2)

#define kTXRequest (tinbus_kFrameSize + 1)
#define kTXIdle (tinbus_kFrameSize + 2)
// #define kTXStart (tinbus_kFrameSize + 3)

TinDuino::TinDuino(HardwareSerial &serial = Serial, unsigned long baud)
    : serialPort{serial}, serialBaud{baud} {}

volatile unsigned long TinDuino::rxActiveMicros = 0;

void TinDuino::externalInterrupt(void) { rxActiveMicros = micros(); }

void TinDuino::begin() {
  serialPort.begin(serialBaud);
  pinMode(TinDuino_kExternalInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(TinDuino_kExternalInterruptPin),
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
      // allow for bus to respond and trigger update of rxActiveMicros
      delayMicroseconds(500);
      return tinbus_kWriteBusy;
    } else {
      txIndex = kTXIdle;
    }
  }

  if (txIndex < tinbus_kFrameSize) {
    unsigned char txData = ((char *)&txFrame)[txIndex];
    if (serialPort.available() > 0) {
      unsigned char rxData = serialPort.read();
      digitalWrite(9, HIGH);
      delayMicroseconds(500);
      digitalWrite(9, LOW);
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

// int TinDuino::sendByte(unsigned char byte) {
//   serialPort.flush(); // wait for any previous tx data to be sent
//   unsigned long time = micros();
//   serialPort.write(byte);
//   // Serial.println(byte, HEX);
//   while (micros() - time < TinDuino_kInterFrameMicros) {
//     if (serialPort.available() > 0) {
//       if (serialPort.read() != byte) {
//         return tinbus_kWriteCollision;
//       } else {
//         return tinbus_kOK;
//       }
//     }
//   }
//   return tinbus_kWriteTimeout;
// }

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

//
//
//
//   // has less than interframe period elapsed since bus activity
//   if (micros() - rxActiveMicros < TinDuino_kInterFrameMicros) {
//     return tinbus_kWriteBusy;
//   }
//   // is bus active now
//   if (digitalRead(TinDuino_kExternalInterruptPin) == LOW) {
//     return tinbus_kWriteBusy;
//   }
//
//   // set start byte
//   txFrame->start = 0;
//   // set sequence number
//   txFrame->sequence = ++sequence;
//   // calculate and set crc
//   unsigned char crc = tinbus_crcFrame(txFrame);
//   txFrame->crc = crc;
//
//   // send frame
//   unsigned char txCount = 0;
//   unsigned char *data = (unsigned char *)txFrame;
//   while (txCount++ < tinbus_kFrameSize) {
//     int err = sendByte(*data++);
//     if (err != tinbus_kOK) {
//       return err;
//     }
//   }
//   return tinbus_kOK;
// }

int TinDuino::read(tinbus_frame_t *frame) {
  if (rxIndex == kRXDataReady) {
    memcpy(frame, &rxFrame, tinbus_kFrameSize);
    rxIndex = kRXDone;
    return tinbus_kOK;
  }
  return tinbus_kReadNoData;
}

// static unsigned char count = 0;
// static tinbus_frame_t frame;
// if(micros() - rxActiveMicros > TinDuino_kInterFrameMicros){
//   count = 0;
//   return tinbus_kReadNoData;
// }
// if (serialPort.available() > 0){
//   if (count < tinbus_kFrameSize) {
//     ((char *)&frame)[count++] = serialPort.read();
//   } else {
//     return tinbus_kReadOverunError;
//   }
//   if(count == tinbus_kFrameSize){
//     if(frame.crc == tinbus_crcFrame(&frame)){
//       unsigned char expectedSequence = sequence + 1;
//       sequence = frame.sequence;
//       if(sequence != expectedSequence){
//         return tinbus_kReadSequenceError;
//       } else {
//         memcpy(rxFrame, &frame, tinbus_kFrameSize);
//         return tinbus_kOK;
//       }
//     } else {
//       return tinbus_kReadCRCError;
//     }
//   }
// }
// return tinbus_kReadNoData;
// }

// unsigned char rxCount = 0;
// unsigned char *frame = (unsigned char *)rxFrame;
//
// while (micros() - rxActiveMicros < TinDuino_kInterFrameMicros) {
//   if (serialPort.available() > 0) {
//     if (rxCount++ < tinbus_kFrameSize) {
//       *frame++ = serialPort.read();
//     } else {
//       return tinbus_kReadOverunError;
//     }
//     if(rxCount == tinbus_kFrameSize){
//       if(rxFrame->crc == tinbus_crcFrame(rxFrame)){
//         unsigned char expectedSequence = sequence + 1;
//         sequence = rxFrame->sequence;
//         if(sequence != expectedSequence){
//           return tinbus_kReadSequenceError;
//         } else {
//           return tinbus_kOK;
//         }
//       } else {
//         return tinbus_kReadCRCError;
//       }
//     }
//   }
// }
