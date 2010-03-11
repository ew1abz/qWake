#include <windows.h>
//#pragma hdrstop

#include "comport_win.h"

//#pragma package(smart_init)

//------------------------------ Constants: ---------------------------------

#define BAUD  19200         //default baudrate
#define TIMEOUT 100         //default timeout, mS

const char PortPref[] = "\\\\.\\COM"; //port name prefix

//------------------------------ Variables: ---------------------------------

HANDLE        hCom;         //COM handle
DCB           dcb;          //COM device control block
DCB           dcbc;         //DCB copy
COMMTIMEOUTS  ComTo;        //COM timeouts
COMMTIMEOUTS  ComToc;       //COM timeouts copy
bool          Platform_NT;  //platform NT flag
bool          RtsToggle;    //RTS toggle mode flag (not NT systems)
WORD          BaseAddress;  //UART base address (not NT systems)
char          PortName[42]; //port name string

//------------------------ Functions prototypes: ----------------------------

LPSTR __fastcall GetPortName(DWORD PortNum); //get port name by number
BYTE __fastcall PortIn(WORD Address);        //io port input (not NT systems)

//---------------------- Get port name by number: --------------------------

LPSTR __fastcall GetPortName(DWORD PortNum)
{
  char PortNums[33];
  strcpy(PortName, PortPref);
  strcat(PortName, itoa(PortNum, PortNums, 10));
  return(PortName);
}

//------------------- IO port input (not NT systems): ----------------------

BYTE __fastcall PortIn(WORD Address)
{
/*
  int result;
  int addr = Address;
 __asm__ __volatile__
  (
    "mov %%dx,%1\n\t"
    "in  %%al,%%dx\n\t"
    "mov %0,%%al\n\t"
    :"=r"(result)      // output
    :"r"(addr)         // input
    :"%dx","%al"       // clobbered register
    );
  return(result);
 asm
  {
    mov dx,Address
    in  al,dx
  }
  return(_AL);
  */
  return 0;
}

//-------------------- Access check for COM resource: ----------------------

bool __fastcall AccessPort(DWORD PortNum)
{
#ifdef __MINGW32__
    HANDLE hTemp = CreateFileA(GetPortName(PortNum),
#else
    HANDLE hTemp = CreateFile(GetPortName(PortNum),
#endif
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);
  if (hTemp != INVALID_HANDLE_VALUE)
  { CloseHandle(hTemp); return(1); }
  else return(0);
}

//----------------- Open COM for non overlapped operations: -----------------

int portOpen(const char *portName)
{
  OSVERSIONINFO Ver;
  Ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&Ver);
  Platform_NT = Ver.dwPlatformId == VER_PLATFORM_WIN32_NT;
#ifdef __MINGW32__
  hCom = CreateFileA(portName,
#else
  hCom = CreateFile(portName,
#endif
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);
  if (hCom == INVALID_HANDLE_VALUE)
  { hCom = NULL;   return(EXIT_FAILURE); }           //port open error
  if (!Platform_NT)
  {
    //Undocumented function returns the base address in edx
   // EscapeCommFunction(hCom, 10);
   // __asm__ __volatile__
   // (
   // "mov %0,%%dx\n\t"
   // :"=r"(BaseAddress)        /* output */
   // :                         /* no input */
   // :"%dx"                    /* clobbered register */
   // );
    //BaseAddress = _DX;
  }
  if (!GetCommState(hCom, &dcb))       return(0); //get state error
  if (!GetCommTimeouts(hCom, &ComTo))  return(0); //get timeouts error
  dcbc = dcb; ComToc = ComTo;                     //save dcb and timeouts
  dcb.BaudRate = BAUD;                    //set boud rate
  dcb.fBinary = 1;                        //binary mode
  dcb.fParity = 0;                        //disable parity checking
  dcb.fOutxCtsFlow = 0;                   //CTS output flow control off
  dcb.fOutxDsrFlow = 0;                   //DSR output flow control
  dcb.fDtrControl = DTR_CONTROL_DISABLE;  //default DTR control
  dcb.fDsrSensitivity = 0;                //DSR sensitivity off
  //dcb.fTXContinueOnXoff = ;             //XOFF continues Tx
  dcb.fOutX = 0;                          //XON/XOFF out flow control off
  dcb.fInX = 0;                           //XON/XOFF in flow control off
  dcb.fErrorChar = 0;                     //disable error replacement
  dcb.fNull = 0;                          //disable null stripping
  dcb.fRtsControl = RTS_CONTROL_ENABLE;   //default RTS control
  dcb.fAbortOnError = 0;                  //abort reads/writes on error off
  //dcb.XonLim = ;                        //transmit XON threshold
  //dcb.XoffLim = ;                       //transmit XOFF threshold
  dcb.ByteSize = 8;                       //set byte size
  dcb.Parity = NOPARITY;                  //set no parity
  dcb.StopBits = ONESTOPBIT;              //set one stop bit
  //dcb.XonChar = ;                       //Tx and Rx XON character
  //dcb.XoffChar = ;                      //Tx and Rx XOFF character
  //dcb.ErrorChar = ;                     //error replacement character
  //dcb.EofChar = ;                       //end of input character
  //dcb.EvtChar = ;                       //received event character
  RtsToggle = 0;
  if (!SetCommState(hCom, &dcb))       return(EXIT_FAILURE); //set state error
  if (!SetupComm(hCom, 512, 512))      return(EXIT_FAILURE); //setup error
  ComTo.ReadIntervalTimeout = MAXDWORD;
  ComTo.ReadTotalTimeoutMultiplier = MAXDWORD;
  ComTo.ReadTotalTimeoutConstant = TIMEOUT;
  ComTo.WriteTotalTimeoutMultiplier = 0;
  ComTo.WriteTotalTimeoutConstant = TIMEOUT;
  if (!SetCommTimeouts(hCom, &ComTo))  return(EXIT_FAILURE); //set timeouts error
  if (!PurgePort())                    return(EXIT_FAILURE); //purge com error
  return(EXIT_SUCCESS);
}

//----------------------------- Set baud rate: ------------------------------
int portSetOptions(long rate,char par)
{
  dcb.BaudRate = rate;                   //set baud rate
  return(SetCommState(hCom, &dcb));      //set state
}

//-------------------------- Set receive timeout: ---------------------------

int SetRxTo(int to)
{
  ComTo.ReadTotalTimeoutConstant = to;
  return(!SetCommTimeouts(hCom, &ComTo)); //set timeout
}

//---------------------------- Set COM mode: --------------------------------

bool __fastcall SetupPort(DWORD RtsMode, DWORD DtrMode)
{
  dcb.fDtrControl = DtrMode;
  RtsToggle = RtsMode == RTS_CONTROL_TOGGLE;
  if(RtsToggle)
    if(Platform_NT)
      dcb.fRtsControl = RTS_CONTROL_TOGGLE;
    else dcb.fRtsControl = RTS_CONTROL_DISABLE;
  else dcb.fRtsControl = RtsMode;
  if (!SetCommState(hCom, &dcb))       return(0); //set state error
  return(1);
}

//----------------------------- Get handle: ---------------------------------

HANDLE __fastcall PortGetHandle(void)
{
  return(hCom);
}

//----------------------------- Close COM: ----------------------------------
void portClose()
{
  if(hCom)
  {
    SetCommState(hCom, &dcbc);
    SetCommTimeouts(hCom, &ComToc);
    CloseHandle(hCom);
    hCom = NULL;
  }
}

bool __fastcall ClosePort(void)
{
  if(hCom)
  {
    SetCommState(hCom, &dcbc);
    SetCommTimeouts(hCom, &ComToc);
    bool res = CloseHandle(hCom);
    hCom = NULL;
    return(res);
  }
  return(0);
}

//------------- Purge COM: terminates TX and RX and clears buffers: ---------

int PurgePort(void)
{
  return(PurgeComm(hCom, PURGE_TXABORT |
                         PURGE_RXABORT |
                         PURGE_TXCLEAR |
                         PURGE_RXCLEAR));
}

//---------------------------- Transmit buffer: -----------------------------
int portWrite(unsigned char *buf, int size)
{
  DWORD r;
  if(!Platform_NT && RtsToggle)
  {
    EscapeCommFunction(hCom, SETRTS);           //set RTS (not NT systems)
  }
  if (!WriteFile(hCom, buf, size, &r, NULL)) return -1;  //TX frame
  if(!Platform_NT && RtsToggle)
  {
    while(!(PortIn(BaseAddress + 5) & 0x40));
    EscapeCommFunction(hCom, CLRRTS);           //clear RTS (not NT systems)
  }
  if (r != (unsigned int)size) return -2;
  else return r;
}

int TxBuff(unsigned char *buff, int j)
{
  DWORD r;
  if(!Platform_NT && RtsToggle)
  {
    EscapeCommFunction(hCom, SETRTS);           //set RTS (not NT systems)
  }
  bool x = WriteFile(hCom, buff, j, &r, NULL);  //TX frame
  if(!Platform_NT && RtsToggle)
  {
    while(!(PortIn(BaseAddress + 5) & 0x40));
    EscapeCommFunction(hCom, CLRRTS);           //clear RTS (not NT systems)
  }
  return(!(x && (r == (unsigned int)j)));
}

//------------------------------ Receive byte: ------------------------------
int portRead(unsigned char *buf, int size, int timeout)
{
  DWORD r;
  if (!ReadFile(hCom, buf, size, &r, NULL)) return -1;         //RX byte
  return r;
}

int RxByte(unsigned char *b)
{
  DWORD r; bool x;
  x =  ReadFile(hCom, b, 1, &r, NULL);         //RX byte
  return(!(x && (r==1)));
}

//---------------------------------------------------------------------------

