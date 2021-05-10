#ifndef BMS_MESSAGE_H
#define BMS_MESSAGE_H

#include "msg.h"

#define BMS_VBAT {0x8000, 0, 16, MSG_MILLI}
#define BMS_IBAT {0x8000, 16, 16, MSG_CENTI}
#define BMS_VTRG {0x8000, 32, 8, MSG_MILLI}
#define BMS_ITRG {0x8000, 40, 8, MSG_UNIT}
#define BMS_VRNG {0x8000, 48, 8, MSG_MILLI}
#define BMS_TBAT {0x8000, 56, 8, MSG_UNIT}

#endif // BMS_MESSAGE_H



// #define BMS_NAMES {     \
//   {"Vbat", BMS_VBAT},   \
//   {"Ibat", BMS_IBAT },  \
//   {"Vtrg", BMS_VTRG },  \
//   {"Itrg", BMS_ITRG },  \
//   {"Vrng", BMS_VRNG },  \
//   {"Tbat", BMS_TBAT },  \
// }

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
