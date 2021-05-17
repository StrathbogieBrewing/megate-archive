#ifndef TINBUS_H
#define TINBUS_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
  tinbus_kOK = 0,
  tinbus_kWriteBusy,
  tinbus_kWriteCollision,
  tinbus_kWriteTimeout,
  tinbus_kWriteComplete,
  tinbus_kReadNoData,
  tinbus_kReadCRCError,
  tinbus_kReadSequenceError,
  tinbus_kReadOverunError,
};

#define tinbus_kDataSize (8)
#define tinbus_kStart (0)

typedef struct {
  unsigned char start;
  unsigned char msgID;
  unsigned char sequence;
  unsigned char data[tinbus_kDataSize];
  unsigned char crc;
} tinbus_frame_t;

#define tinbus_kFrameSize (sizeof(tinbus_frame_t))

unsigned char tinbus_crcFrame(const tinbus_frame_t *frame);
unsigned char tinbus_crcByte(unsigned char crc, unsigned char data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINBUS_H
