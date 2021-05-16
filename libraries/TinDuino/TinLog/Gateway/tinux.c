#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "tinux.h"

#define kBufferSize (256)

static int fd = -1;
static struct termios original_termios;

int tinux_open(char *port) {
  // open the serial port
  int rc = -1;
  struct termios termios;

  fd = open(port, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    fprintf(stderr, "TTY Device \"%s\" could not be opened\n", port);
    return -1;
  }
  if (!isatty(fd)) {
    fprintf(stderr, "TTY Device \"%s\" is not a tty\n", port);
    return -1;
  }
  // initialize terminal modes
  rc = tcgetattr(fd, &termios);
  if (rc < 0) {
    fprintf(stderr, "TTY Device tcgetattr() failed\n");
    return -1;
  }
  original_termios = termios;

  cfmakeraw(&termios);
  termios.c_cc[VMIN] = 1;
  termios.c_cc[VTIME] = 0;

  if (cfsetospeed(&termios, B1200) < 0) {
    fprintf(stderr, "TTY Device cfsetospeed() failed\n");
    return -1;
  }
  if (cfsetispeed(&termios, B1200) < 0) {
    fprintf(stderr, "TTY Device cfsetispeed() failed\n");
    return -1;
  }
  rc = tcsetattr(fd, TCSANOW, &termios);
  if (rc < 0) {
    fprintf(stderr, "TTY Device tcsetattr() failed\n");
    return -1;
  }
  return 0;
}

int tinux_read(tinbus_frame_t *rxFrame) {
  // assume unable to reliably meet timing requirements in linux
  static char ringBuffer[kBufferSize];
  static unsigned char rxHead = 0;
  static unsigned char rxTail = 0;

  if (fd < 0) {
    return tinbus_kReadNoData;
  }
  // read 2 frames into ring buffer
  while ((unsigned char)(rxHead - rxTail)< tinbus_kFrameSize * 2){
    read(fd, &ringBuffer[rxHead++], 1);
  }
  while((unsigned char)(rxHead - rxTail) >= tinbus_kFrameSize){
    if(ringBuffer[rxTail] == tinbus_kStart){
      // this could be the start of a valid frame
      unsigned char *data = (unsigned char *)rxFrame;
      *data++ = tinbus_kStart;
      unsigned char crc = 0;
      unsigned char rxIndex = rxTail + 1;
      unsigned char bytes = tinbus_kFrameSize - 2;
      while (bytes--) {
        *data = ringBuffer[rxIndex++];
        crc = tinbus_crcByte(crc, *data++);
      }
      *data = ringBuffer[rxIndex++];
      if(crc == *data){
        rxTail = rxIndex;
        return tinbus_kOK;
      }
    }
    // not the start of a valid frame
    rxTail++;
  }
  // reset buffer - too much corrupt data
  rxHead = 0;
  rxTail = 0;
  return tinbus_kReadOverunError;
}

void tinux_close(void) {
  if (fd != -1) {
    close(fd);
    fd = -1;
  }
}
