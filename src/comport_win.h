#ifndef COMPORT_WIN_H
#define COMPORT_WIN_H

#include <windows.h>

int portOpen(const char* portName);
void portSetOptions(long rate,char par);
void portClose();
const char * portGetError();

int portWrite(unsigned char *buf, int size);
int portRead(unsigned char *buf, int size, int timeout);

#endif




#ifdef rs_io_H

#include <windows.h>

int __fastcall OpenPort(DWORD PortNum);
int __fastcall ClosePort(void);
int TxBuff(unsigned char *buff, int j);

bool __fastcall SetBaud(DWORD Baud);
int SetRxTo(int RxTo);
bool __fastcall SetupPort(DWORD RtsMode, DWORD DtrMode);
HANDLE __fastcall PortGetHandle(void);
bool __fastcall PurgePort(void);
int RxByte(unsigned char *b);
bool __fastcall AccessPort(DWORD PortNum);

#endif

