#ifndef WAKE_H
#define WAKE_H

#include "../qextserialport/qextserialport.h"

struct wake_cmd
  {
  char name[16];         // for error messages
  unsigned char code;
  unsigned char need_tx;
  unsigned char need_rx;
  int timeout;
  };

void wake_init (QextSerialPort *p);
void wakeSetMonitorMode(bool mode);
int wake_tx_frame(unsigned char addr, unsigned char cmd, unsigned char need_tx, const char *data);
int wake_rx_frame(int to, unsigned char *addr, unsigned char *cmd, unsigned char *real_rx, char *data);
int wake_get_tx_raw_buffer(char * buf);
int wake_get_rx_raw_buffer(char * buf);

int WakeCmdExe(unsigned char addr, struct wake_cmd cmd, unsigned char *real_rx, char *data);
//void WakeGetLastError(const char * error);

#endif // WAKE_H
