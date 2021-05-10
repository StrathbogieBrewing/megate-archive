
enum {
  META_MILLI = 0, // make fixed point with 3 decimal places
  META_CENTI,     // make fixed point with 2 decimal places
  META_DECI,      // make fixed point with 1 decimal places
  META_UNIT,      // display at least one digit
  META_TWODIGITS, // display at least two digit
  META_TIME,      // display as a time
  META_BOOL,      // display as on / off

  META_SIGNED = 0x80
};

typedef struct {
  const unsigned int messageID;
  const unsigned char dataBitOffset;
  const unsigned char dataBitCount;
  const unsigned char dataFormat;
} meta_pack_t;

typedef struct {
  const char *name;
  const meta_pack_t *meta;
} meta_name_t;


#include "meta.h"



// meta_t vBat = {0x8000, META_TXT("Vbat"), 0, 16, MSG_MILLI};
// meta_t iBat = {0x8000, META_TXT("Ibat"), 16, 16, MSG_CENTI & MSG_SIGNED};
// meta_t vTrg = {0x8000, META_TXT("Vtrg"), 32, 8, MSG_MILLI};
// meta_t iTrg = {0x8000, META_TXT("Itrg"), 40, 8, MSG_UNIT };
// meta_t vRng = {0x8000, META_TXT("Vrng"), 48, 8, MSG_MILLI};
// meta_t tBat = {0x8000, META_TXT("Tbat"), 56, 8, MSG_UNIT};
