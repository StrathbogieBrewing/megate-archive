#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <time.h>

#include "tinux.h"

#define kProgramName (0)
#define kSerialDevice (1)

#define kUDPPort (12345)

#define kBufferSize (256)

static volatile int keepRunning = 1;
void intHandler(int dummy) { keepRunning = 0; }

struct sockaddr_in si_other;
int s, i;

void sendUDP(char *data, int size) {
  if (sendto(s, data, size, 0, (struct sockaddr *)&si_other,
             sizeof(si_other)) == -1) {
    fprintf(stderr, "UDP Socket send failed\n");
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stdout, "Usage: %s <serial device>\n", argv[kProgramName]);
    return EXIT_FAILURE;
  }

  if (tinux_open(argv[kSerialDevice]) == -1) {
    return EXIT_FAILURE;
  }

  // initialise udp port
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    fprintf(stderr, "UDP Socket could not be opened\n");
    return EXIT_FAILURE;
  }

  int yes = 1;
  if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes)) ==
      -1) {
    fprintf(stderr, "UDP Socket could not set options\n");
    return EXIT_FAILURE;
  }

  memset((char *)&si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(kUDPPort);
  si_other.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  while (keepRunning) {
    tinbus_frame_t rxFrame;
    if (tinux_read(&rxFrame) == tinbus_kOK) {
      sendUDP((char*)&rxFrame, tinbus_kFrameSize);
    }
  }

  // teardown ports
  tinux_close();
  close(s);
  fprintf(stdout, "Exit\n");

  return EXIT_SUCCESS;
}

// char str[kBufferSize] = {0};
// int index = 0;
// while(index < kNameCount){
//   int value;
//   int format = msg_unpack(&rxFrame, &msgNames[index].pack, &value);
//   if(format != MSG_NULL){
//     if(index) strcat(str, ",");
//     sprintf(str + strlen(str), "%s=%d", msgNames[index].name, value);
//   }
//   index++;
// }
// strcat(str, "\n");
// sendUDP(str);


// unsigned char index = 0;
// unsigned char *frame = (unsigned char *)&rxFrame;
// char str[kBufferSize] = {0};
// sprintf(str, "RX Data : > ");
// while (index++ < tinbus_kFrameSize) {
//   sprintf(str + strlen(str), "0x%2.2X ", *frame++);
// }
// sprintf(str + strlen(str), "<\n");
// sendUDP(str);
