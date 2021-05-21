#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <time.h>

#include "tinux.h"
#include "log.h"

// #include "msg_solar.h"
// msg_name_t msgNames[] = MSG_NAMES;
// #define msgCount (sizeof(msgNames) / sizeof(msg_name_t))

// static int crcErrorCount = 0;
// static int overunErrorCount = 0;
// static int sequenceErrorCount = 0;
// static int messageErrorCount = 0;
//
// msg_pack_t crcError = LOG_CRCE;
// msg_pack_t overunError = LOG_SEQE;
// msg_pack_t sequenceError = LOG_OVRE;
// msg_pack_t messageError = LOG_MSGE;

#define kProgramName (0)
#define kSerialDevice (1)
#define kLogPath (2)

#define kBufferSize (256)

static volatile int keepRunning = 1;
void intHandler(int dummy) { keepRunning = 0; }

// void logError(void){
//   tinframe_t errorFrame;
//   msg_pack(&errorFrame, &crcError, crcErrorCount);
//   msg_pack(&errorFrame, &overunError, overunErrorCount);
//   msg_pack(&errorFrame, &sequenceError, sequenceErrorCount);
//   msg_pack(&errorFrame, &messageError, messageErrorCount);
//   log_commit(&errorFrame);
// }

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stdout, "Usage: %s <serial device> <log path>\n", argv[kProgramName]);
    return EXIT_FAILURE;
  }
  if (tinux_open(argv[kSerialDevice]) == -1) {
    return EXIT_FAILURE;
  }
  log_begin(argv[kLogPath]);
  unsigned char sequence = 0;
  while (keepRunning) {
    tinframe_t rxFrame;
    int result = tinux_read(&rxFrame);
    if (result == tinux_kOK) {
      log_commit(&rxFrame);
      // char str[kBufferSize] = {0};
      // sprintf(str + strlen(str), "msg : 0x%2.2X\tseq : %u  \t",
      //         rxFrame.data[MSG_ID], rxFrame.data[MSG_SEQ]);
      // int found = 0;
      // int index = 0;
      // while (index < msgCount) {
      //   int value;
      //   int format = msg_unpack(&rxFrame, &msgNames[index].pack, &value);
      //   if (format != MSG_NULL) {
      //     if (found)
      //       sprintf(str + strlen(str), ", \t");
      //     found++;
      //     char valueBuffer[kBufferSize] = {0};
      //     msg_format(&rxFrame, &msgNames[index].pack, valueBuffer);
      //     sprintf(str + strlen(str), "%s=%s", msgNames[index].name, valueBuffer);
      //   }
      //   index++;
      // }
      // sprintf(str + strlen(str), "\n");
      // fprintf(stdout, "%s", str);
      // if(found == 0){
      //   messageErrorCount++;
      //   logError();
      // }

      // if (rxFrame.data[MSG_SEQ] != (unsigned char)(sequence + 1)) {
      //   sequenceErrorCount++;
      //   logError();
      // }
      // sequence = rxFrame.data[MSG_SEQ];
    // } else if (result == tinux_kReadCRCError) {
    //   crcErrorCount++;
    //   logError();
    // } else if (result == tinux_kReadOverunError) {
    //   overunErrorCount++;
    //   logError();
    }
  }
  // teardown ports
  tinux_close();
  log_end();
  fprintf(stdout, "Exit\n");

  return EXIT_SUCCESS;
}