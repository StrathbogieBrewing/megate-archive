#ifndef BMS_MESSAGE_H
#define BMS_MESSAGE_H

#include "message.h"

#define BMS_VBAT {0x8000, 0, 16, MESSAGE_MILLI}
#define BMS_IBAT {0x8000, 16, 16, MESSAGE_CENTI & MESSAGE_SIGNED}
#define BMS_VTRG {0x8000, 32, 8, MESSAGE_MILLI}
#define BMS_ITRG {0x8000, 40, 8, MESSAGE_UNIT }
#define BMS_VRNG {0x8000, 48, 8, MESSAGE_MILLI}
#define BMS_TBAT {0x8000, 56, 8, MESSAGE_UNIT}

#define BMS_NAMES {     \
  {BMS_VBAT, "Vbat"},   \
  {BMS_IBAT, "Ibat" },  \
  {BMS_VTRG, "Vtrg" },  \
  {BMS_ITRG, "Itrg" },  \
  {BMS_VRNG, "Vrng" },  \
  {BMS_TBAT, "Tbat" },  \
}

#endif // BMS_MESSAGE_H


// // msgID, desccription, bit offset, bit count, format
// #define kBMSItems                                                              \
//   {                                                                            \
//     {0x8000, MSG_TXT("Battery Voltage"), 0, 16, MSG_MILLI},                    \
//     {0x8000, MSG_TXT("Battery Current"), 16, 16, MSG_CENTI & MSG_SIGNED},      \
//     {0x8000, MSG_TXT("Battery Target Voltage"), 32, 8, MSG_MILLI},             \
//     {0x8000, MSG_TXT("Battery Target Current"), 40, 8, MSG_UNIT },             \
//     {0x8000, MSG_TXT("Cell Voltage Range"), 48, 8, MSG_MILLI},                 \
//     {0x8000, MSG_TXT("Battery Temperature"), 56, 8, MSG_UNIT},                 \
//   }
//
// #define kBMSCount (sizeof(bmsItems) / sizeof(dataItem_t))
//
//
// dataElement bmsItems[] = kbmsItems;
