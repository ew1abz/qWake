#ifndef rs_io_H
#define rs_io_H

#include <windows.h>

bool __fastcall AccessPort(DWORD PortNum);
bool __fastcall OpenPort(DWORD PortNum);
bool __fastcall SetBaud(DWORD Baud);
int SetRxTo(int RxTo);
bool __fastcall SetupPort(DWORD RtsMode, DWORD DtrMode);
HANDLE __fastcall PortGetHandle(void);
bool __fastcall ClosePort(void);
bool __fastcall PurgePort(void);
int TxBuff(unsigned char *buff, int j);
int RxByte(unsigned char *b);

#endif

