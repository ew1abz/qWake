#include <stdbool.h>

class Dev
  {
  private:
  #define FRAME     200 //max frame size

  int DevHandle;                 // device handle
  unsigned char TxData[FRAME];   // TX data buffer
  unsigned char RxData[FRAME];   // RX data buffer
  bool CmdExe(unsigned char cmd, unsigned char n);

  public:
  Dev();
  bool GetInfo(char *info);
  bool OpenDevice(char);
  bool CloseDevice(void);
  bool Test();
  bool Write();
  bool Read();
  bool Init();
  };



