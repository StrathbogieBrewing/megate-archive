#include <SPI.h>
#include <avr/wdt.h>
#include <limits.h>

#include "mcp2515.h"
#include "TinDuino.h"
#include "msg_solar.h"

msg_pack_t vbat = BMS_VBAT;
msg_pack_t ibat = BMS_IBAT;
msg_pack_t vtrg = BMS_VTRG;
msg_pack_t itrg = BMS_ITRG;

msg_pack_t cel1 = BMS_CEL1;
msg_pack_t cel2 = BMS_CEL2;
msg_pack_t cel3 = BMS_CEL3;
msg_pack_t cel4 = BMS_CEL4;
msg_pack_t cel5 = BMS_CEL5;
msg_pack_t cel6 = BMS_CEL6;
msg_pack_t cel7 = BMS_CEL7;
msg_pack_t cel8 = BMS_CEL8;
msg_pack_t vbal = BMS_VBAL;
msg_pack_t chah = BMS_CHAH;
msg_pack_t btc1 = BMS_BTC1;
msg_pack_t btc2 = BMS_BTC2;

#define kInterruptPin (2)
TinDuino tinDuino(Serial, kInterruptPin);

unsigned char frameSequence = 0;

struct can_frame canMsg;
MCP2515 mcp2515(6);

const int buzzerPin = 9;
const int ledPin = 5;


#define CANBUS_CAN_ID_SHUNT 40
#define CANBUS_CAN_ID_BMS 300

typedef struct {
  int16_t cellVoltage[8];
  int16_t temperature[2];
  int32_t chargeMilliAmps;
  int16_t balanceVoltage;
} bms_t;

static bms_t bms;

void process(void) {
  // called 4 times per second, triggered by current shunt canbus message
  static int8_t count = 0;

  uint16_t cellMin = 5000;
  uint16_t cellMax = 0;
  uint16_t cellSum = 0;

  // reset watchdog timer when we get a current measurement
  wdt_reset();

  uint8_t i = 8;
  while (i--) {
    uint16_t v = bms.cellVoltage[i];
    if (v > cellMax)
      cellMax = v;
    if (v < cellMin)
      cellMin = v;
    cellSum += v;
  }

  if (cellMax > 0) {
    bms.balanceVoltage = (cellSum / 8) + 2;
    if (bms.balanceVoltage < 3000) {
      bms.balanceVoltage = 3000;
    }
  } else {
    bms.balanceVoltage = 0;
  }

  count++;
  bool buzzer = (count & 0x08) && (bms.chargeMilliAmps < -5000);
  digitalWrite(buzzerPin, buzzer);

  if (count & 0x01) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  int16_t chargeCentiAmps = bms.chargeMilliAmps / 10;

  tinframe_t txFrame;
  // always send battery voltage and current
  msg_pack(&txFrame, &vbat, cellSum);
  msg_pack(&txFrame, &ibat, chargeCentiAmps);
  msg_pack(&txFrame, &vtrg, 26700);
  msg_pack(&txFrame, &itrg, 2000);
  txFrame.data[MSG_SEQ] = 0;
  tinDuino.write(&txFrame);
  frameSequence = txFrame.data[MSG_SEQ];
}

void setup() {
  // enable watchdog timer
  wdt_reset();
  wdt_enable(WDTO_1S);

  tinDuino.begin();

  SPI.begin();

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

#define SCHEDMSK (0xFE)

void loop() {
  // update bus
  if(tinDuino.update() == TinDuino_kWriteComplete){
    // send other parameters when sequence number matches message ID
    tinframe_t txFrame;
    if((frameSequence & SCHEDMSK) == (cel1.msgID & SCHEDMSK)){
      msg_pack(&txFrame, &cel1, bms.cellVoltage[0]);
      msg_pack(&txFrame, &cel2, bms.cellVoltage[1]);
      msg_pack(&txFrame, &cel3, bms.cellVoltage[2]);
      msg_pack(&txFrame, &cel4, bms.cellVoltage[3]);
      tinDuino.write(&txFrame);
    } else if((frameSequence & SCHEDMSK) == (cel5.msgID & SCHEDMSK)){
      msg_pack(&txFrame, &cel5, bms.cellVoltage[4]);
      msg_pack(&txFrame, &cel6, bms.cellVoltage[5]);
      msg_pack(&txFrame, &cel7, bms.cellVoltage[6]);
      msg_pack(&txFrame, &cel8, bms.cellVoltage[7]);
      tinDuino.write(&txFrame);
    } else if((frameSequence & SCHEDMSK) == (vbal.msgID & SCHEDMSK)){
      msg_pack(&txFrame, &vbal, bms.balanceVoltage);
      msg_pack(&txFrame, &chah, bms.cellVoltage[1]);
      msg_pack(&txFrame, &btc1, bms.temperature[0]);
      msg_pack(&txFrame, &btc2, bms.temperature[1]);
      tinDuino.write(&txFrame);
    }
    frameSequence = 0;
  };

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if (canMsg.can_id == (CANBUS_CAN_ID_SHUNT | CAN_EFF_FLAG)) {
      if (canMsg.can_dlc == 8) {
        int32_t current = (int32_t)(((uint32_t)canMsg.data[0] << 16) +
                                    ((uint32_t)canMsg.data[1] << 8) +
                                    (uint32_t)canMsg.data[2]) -
                          8388608L;
        bms.chargeMilliAmps = current;

        process();

        // send can message
        canMsg.can_id = (CANBUS_CAN_ID_BMS | CAN_EFF_FLAG);
        canMsg.can_dlc = 2;
        canMsg.data[0] = bms.balanceVoltage >> 8;
        canMsg.data[1] = bms.balanceVoltage;
        mcp2515.sendMessage(&canMsg);
      }
    }
    if (canMsg.can_id == ((CANBUS_CAN_ID_BMS + 1) | CAN_EFF_FLAG)) {
      if (canMsg.can_dlc == 8) {
        // update cell voltages
        bms.cellVoltage[0] =
            ((uint16_t)canMsg.data[0] << 8) + (uint16_t)canMsg.data[1];
        bms.cellVoltage[1] =
            ((uint16_t)canMsg.data[2] << 8) + (uint16_t)canMsg.data[3];
        bms.cellVoltage[2] =
            ((uint16_t)canMsg.data[4] << 8) + (uint16_t)canMsg.data[5];
        bms.cellVoltage[3] =
            ((uint16_t)canMsg.data[6] << 8) + (uint16_t)canMsg.data[7];
      }
    }
    if (canMsg.can_id == ((CANBUS_CAN_ID_BMS + 2) | CAN_EFF_FLAG)) {
      if (canMsg.can_dlc == 8) {
        // update cell voltages
        bms.cellVoltage[4] =
            ((uint16_t)canMsg.data[0] << 8) + (uint16_t)canMsg.data[1];
        bms.cellVoltage[5] =
            ((uint16_t)canMsg.data[2] << 8) + (uint16_t)canMsg.data[3];
        bms.cellVoltage[6] =
            ((uint16_t)canMsg.data[4] << 8) + (uint16_t)canMsg.data[5];
        bms.cellVoltage[7] =
            ((uint16_t)canMsg.data[6] << 8) + (uint16_t)canMsg.data[7];
      }
    }
    if (canMsg.can_id == ((CANBUS_CAN_ID_BMS + 4) | CAN_EFF_FLAG)) {
      if (canMsg.can_dlc == 8) {
        // update temperatures
        bms.temperature[0] = (int16_t)canMsg.data[0] - 40;
        bms.temperature[1] = (int16_t)canMsg.data[1] - 40;
      }
    }
  }
}
