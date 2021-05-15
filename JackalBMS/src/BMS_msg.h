#ifndef BMS_MESSAGE_H
#define BMS_MESSAGE_H

#include "msg.h"

#define BMS_VBAT {0x80, 0, 16, MSG_MILLI}
#define BMS_IBAT {0x80, 16, 16, MSG_CENTI}
#define BMS_VTRG {0x80, 32, 8, MSG_MILLI}
#define BMS_ITRG {0x80, 40, 8, MSG_UNIT}
#define BMS_VRNG {0x80, 48, 8, MSG_MILLI}
#define BMS_TBAT {0x80, 56, 8, MSG_UNIT}

#define BMS_NAMES {     \
  {"Vbat", BMS_VBAT},   \
  {"Ibat", BMS_IBAT },  \
  {"Vtrg", BMS_VTRG },  \
  {"Itrg", BMS_ITRG },  \
  {"Vrng", BMS_VRNG },  \
  {"Tbat", BMS_TBAT },  \
}

#endif // BMS_MESSAGE_H
