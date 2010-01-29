#include "i2c_adapter.h"
#include "wake.h"
#include <stdlib.h>
#include <string.h>
#include "rs_io.h"

//typedef char SStr[32];

//---------------------------- Constants: -----------------------------------
//Wake protocol constantes:


//Wake protocol commands:

#define CMD_NOP        0 //no operation
#define CMD_ERR        1 //get error code
#define CMD_ECHO       2 //get echo
#define CMD_INFO       3 //get information
#define CMD_SETADDR    4 //set net address
#define CMD_GETADDR    5 //get net address

//Specific FN-603 commands:

#define CMD_SETM       8 //set mode
#define CMD_GETM       9 //get mode

//Unknown command:

#define CMD_UNKNOWN   14 //unknown command

#define MAX_CMD       CMD_UNKNOWN

const char des[] = "i2c-adapter";
//const int baud = 19200;                       // baud rate
const int baud = 115200;                       // baud rate
const unsigned long RxTo = 200;               // RX timeout, mS

//------------------------------ Variables: ---------------------------------



//-------------------------- Execute command: -------------------------------

bool Dev::CmdExe(unsigned char cmd, unsigned char n)
{
  unsigned char addr;
  memset(RxData, FRAME, 0);                                              //clear RX data
  if (!DevHandle) return EXIT_FAILURE;                                   //if port is opened
  if (wake_tx_frame(0, cmd, n, TxData) < 0) return EXIT_FAILURE;   //transmit frame
  if (wake_rx_frame(100, &addr, &cmd, &n, RxData) < 0) return EXIT_FAILURE; //receive frame
  return EXIT_SUCCESS;
}

//---------------------------------------------------------------------------
//-------------------------- Public functions -----------------------------
//---------------------------------------------------------------------------
Dev::Dev()
{
  wake_init(SetRxTo,  TxBuff, RxByte);
}

//---------------------------- Open device: ---------------------------------

bool Dev::OpenDevice(char port)
{
  if (OpenPort(port) == 0) return EXIT_FAILURE;
  if (SetBaud(baud) == 0) return EXIT_FAILURE;
  DevHandle = 1;
  return EXIT_SUCCESS;
}

//---------------------------- Close device: --------------------------------

bool Dev::CloseDevice(void)
{
  ClosePort();
  return EXIT_SUCCESS;
}

//------------------------------ CMD_INFO: ----------------------------------

bool Dev::GetInfo(char *info)
{
  if (CmdExe(CMD_INFO, 0) == EXIT_FAILURE) return EXIT_FAILURE;
  strcpy (info,(char*)RxData);
  return EXIT_SUCCESS;
}

////------------------------------- C_GetT: -----------------------------------
//
//bool FN603_GetT(int &t1, int &t2, int &t3, int &t4, int &t5, int &t6)
//{
//  bool rx = CmdExe(CMD_GETT, 0);
//  t1 = (short)(RxData[1]  | (RxData[2]  << 8));
//  t2 = (short)(RxData[3]  | (RxData[4]  << 8));
//  t3 = (short)(RxData[5]  | (RxData[6]  << 8));
//  t4 = (short)(RxData[7]  | (RxData[8]  << 8));
//  t5 = (short)(RxData[9]  | (RxData[10] << 8));
//  t6 = (short)(RxData[11] | (RxData[12] << 8));
//  return(rx);
//}
//
////------------------------------- C_SetM: -----------------------------------
//
//bool FN603_SetM(int chan, int mode, int Tmin, int Pmin,
//                       int Tmax, int Pmax, int At, int Av, int Pman)
//{
//  TxData[0] =  (unsigned char)  chan & 0xFF;
//  TxData[1] =  (unsigned char)  mode & 0xFF;
//  TxData[2] =  (unsigned char)  Tmin & 0xFF;
//  TxData[3] =  (unsigned char) (Tmin >> 8) & 0xFF;
//  TxData[4] =  (unsigned char)  Pmin & 0xFF;
//  TxData[5] =  (unsigned char)  Tmax & 0xFF;
//  TxData[6] =  (unsigned char) (Tmax >> 8) & 0xFF;
//  TxData[7] =  (unsigned char)  Pmax & 0xFF;
//  TxData[8] =  (unsigned char)  At & 0xFF;
//  TxData[9] =  (unsigned char) (At >> 8) & 0xFF;
//  TxData[10] = (unsigned char)  Av & 0xFF;
//  TxData[11] = (unsigned char) (Av >> 8) & 0xFF;
//  TxData[12] = (unsigned char)  Pman & 0xFF;
//  bool rx = CmdExe(CMD_SETM, 13);
//  return(rx);
//}

