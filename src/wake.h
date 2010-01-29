#ifndef WAKE_H
#define WAKE_H

struct wake_cmd
  {
  char name[16];         // for error messages
  unsigned char code;
  unsigned char need_tx;
  unsigned char need_rx;
  int timeout;
  };

void wake_init (int(*f_set_rx_to)(int),  int(*f_tx_buffer)(unsigned char*, int), int (*f_rx_byte)(unsigned char*));
int wake_tx_frame(unsigned char addr, unsigned char cmd, unsigned char need_tx, unsigned char *data);
int wake_rx_frame(int to, unsigned char *addr, unsigned char *cmd, unsigned char *real_rx, unsigned char *data);
int wake_get_tx_buffer(unsigned char * buf);

bool WakeCmdExe(unsigned char addr, struct wake_cmd cmd, unsigned char *real_rx, char *data);
//void WakeGetLastError(const char * error);

#endif // WAKE_H
