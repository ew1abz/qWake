//#include "frame.h"
//#include "ftdidev.h"
//#include <stdlib.h>
//#include <string.h>

#include "wake.h"
#include <string.h>
#include <stdlib.h>
#include "rs_io.h"


//---------------------------- Constants: -----------------------------------

//Special commands:
#define CMD_NOP  0    //no operation
#define CMD_ERR  1    //get error code
#define CMD_ECHO 2    //get echo
#define CMD_INFO 3    //get information

//Wake protocol errors:
#define ERR_NO   0    //no error
#define ERR_TX   1    //IO error
#define ERR_BU   2    //device busy error
#define ERR_RE   3    //device not ready
#define ERR_PA   4    //parameters value error
#define ERR_NR   5    //device not responding

//Unknown error:
#define ERR_UK   6    //unknown error
#define MAX_ERR  6

// fo debug only!
unsigned char buffer_copy[518];
int real_tx;

//typedef char SSTR[32];

//Errors names:
//const SSTR ErrorMsg[MAX_ERR + 1] =
//  { "",
//    "invalid packet.",
//    "device busy.",
//    "device not ready.",
//    "invalid parameters.",
//    "device not responding.",
//    "unknown."
//  };
//
////Special commands names:
//const SSTR CmdNames[2] =
//  { "",
//    "IO error: "
//  };
//
////----------------------------- Variables: ----------------------------------
//static char LastError[64] = "";     //last error string

//---------------------------- Constants: -----------------------------------

#define CRC_INIT 0xDE // CRC Initial value
#define error_return(code, str) do {error_str = (char*)str; return code; } while(0);

char *error_str;

const unsigned char
 FEND  = 0xC0,        // Frame END
 FESC  = 0xDB,        // Frame ESCape
 TFEND = 0xDC,        // Transposed Frame END
 TFESC = 0xDD;        // Transposed Frame ESCape


static int (*set_rx_to)(int);
static int (*tx_buffer)(unsigned char*, int);
static int (*rx_byte)(unsigned char*);

void wake_init (int(*f_set_rx_to)(int),  int(*f_tx_buffer)(unsigned char*, int), int (*f_rx_byte)(unsigned char*))
{
  set_rx_to = f_set_rx_to;
  tx_buffer = f_tx_buffer;
  rx_byte = f_rx_byte;
}

//------------------------------------------------------------------------------

///    Calculate CRC
///    \param b New byte to calculate
///    \param crc Current CRC
static void do_crc(unsigned char b, unsigned char *crc)
{
  for (int i = 0; i < 8; i++)
  {
   if (((b ^ *crc) & 1) != 0)
     *crc = ((*crc ^ 0x18) >> 1) | 0x80;
       else *crc = (*crc >> 1) & ~0x80;
   b = b >> 1;
   }
}

//------------------------------------------------------------------------------

///    Do byte stuffing in buffer and update pointer, if needed
///    \param b Byte to tx
///    \param dptr Data pointer
///    \param buff Buffer
static void byte_stuff(unsigned char b, int &bptr, unsigned char *buff)
{
  if ((b == FEND) || (b == FESC))
  {
    buff[bptr++] = FESC;
    buff[bptr++] = (b == FEND)? TFEND : TFESC;
  }
  else buff[bptr++] = b;
}

//------------------------------------------------------------------------------

///    Rx byte with wake destuffing
///    \param b New rx byte
///    \retval  0: all fine
///    \retval -1: rx data failed
///    \retval -2: rx destuffing data failed
///    \retval -3: wrong destuffing data
///    \note Don't update error string, because up-level functions rewrite it
static int wake_rx(unsigned char *b)
{
  if((*rx_byte)(b)) return(-1);
  if(*b == FESC)
  {
    if((*rx_byte)(b)) return(-2);
    if(*b == TFEND) *b = FEND;
      else if(*b == TFESC) *b = FESC;
        else return(-3);
  }
  return(0);
}

//------------------------------------------------------------------------------

///    Transmit frame in wake format
///    \param to
///    \param addr
///    \param cmd
///    \param need_tx
///    \param data
///    \retval  0: all fine
///    \retval -1: ftdidev_tx_buff failed
int wake_tx_frame(unsigned char addr, unsigned char cmd, unsigned char need_tx, unsigned char *data)
{
  unsigned char buff[518];                // буфер для передачи
  int index = 0;                          // указатель буфера
  unsigned char crc = CRC_INIT;           // контрольная сумма

  // fixme: check input params
  buff[index++] = FEND;                   // передача FEND
  do_crc(FEND, &crc);
  if(addr)
  {
    byte_stuff(addr | 0x80, index, buff); // передача адреса
    do_crc(addr & 0x7F, &crc);
  }
  byte_stuff(cmd & 0x7F, index, buff);    // передача команды
  do_crc(cmd & 0x7F, &crc);
  byte_stuff(need_tx, index, buff);       // передача длины пакета
  do_crc(need_tx, &crc);
  for(int i = 0; i < need_tx; i++)
  {
    byte_stuff(data[i], index, buff);     // передача данных
    do_crc(data[i], &crc);
  }
  byte_stuff(crc, index, buff);           // передача CRC
  real_tx = index;
  memcpy((char*)buffer_copy,(const char *)buff,real_tx);
  if ((*tx_buffer)(buff, index))  error_return(-1, "ftdidev_tx_buff failed");
  return 0;
}

int wake_get_tx_buffer(unsigned char * buf)
{
  memcpy((char*)buf,(const char *)buffer_copy,real_tx);
  return real_tx;
}


//------------------------------------------------------------------------------

///    Receive frame in wake format
///    \param to Time out
///    \param addr Wake address
///    \param cmd Wake command
///    \param real_rx Number of recieved bytes
///    \param data Buffer to store data in
///    \retval   0: all fine
///    \retval  -1: ftdidev_set_rx_to failed
///    \retval  -2: ftdidev_rx_byte failed
///    \retval  -3: can't find FEND
///    \retval  -4: can't rx wake_addr
///    \retval  -5: can't rx wake_cmd
///    \retval  -6: can't rx wake_len
///    \retval  -7: can't rx wake_data
///    \retval  -8: wrong wake frame len
///    \retval  -9: can't rx wake_crc
///    \retval -10: wrong wake_crc
int wake_rx_frame(int to, unsigned char *addr, unsigned char *cmd, unsigned char *real_rx, unsigned char *data)
{
  int data_ptr;                            // pointer to data buffer
  unsigned char data_byte;                 // recieved byte
  unsigned char crc = CRC_INIT;            // crc

  if ((*set_rx_to)(to)) error_return(-1, "ftdidev_set_rx_to failed");      // установка таймаута

  for (int i = 0; i < 512 && data_byte != FEND; i++)
    if ((*rx_byte)(&data_byte)) break;
  if (data_byte != FEND) error_return(-3, "can't find FEND");

  do_crc(data_byte, &crc);
  if (wake_rx(&data_byte)) error_return(-4, "can't rx wake_addr");   // прием адреса
  if (data_byte & 0x80)
  {
    *addr = data_byte & 0x7F;
    do_crc(data_byte & 0x7F, &crc);
    if (wake_rx(&data_byte)) error_return(-5, "can't rx wake_cmd");  // прием команды
  }
  else *addr = 0;
  *cmd = data_byte;
  do_crc(data_byte, &crc);
  if (wake_rx(&data_byte)) error_return(-6, "can't rx wake_len");    // прием длины пакета
  *real_rx = data_byte;
  do_crc(data_byte, &crc);
  for (data_ptr = 0; data_ptr < *real_rx; data_ptr++)
  {
    if (wake_rx(&data_byte)) error_return(-7, "can't rx wake_data"); // прием данных
    data[data_ptr] = data_byte;
    do_crc(data_byte, &crc);
  }
  if (data_ptr != *real_rx) error_return(-8, "wrong wake frame len");
  if (wake_rx(&data_byte)) error_return(-9, "can't rx wake_crc");    // прием CRC
  if (data_byte != crc) error_return(-10, "wrong wake_crc");
  return 0;
}

//------------------------------------------------------------------------------

//-------------------------- Report of error: -------------------------------

//bool __fastcall ErrorReport(int cmd, int errc, LPCSTR &lpcName)
//bool ErrorReport(int cmd, int errc, const char * name)
//{
//  if(cmd == CMD_ECHO || cmd == CMD_INFO) errc = ERR_NO;
//  if(errc == ERR_NO) cmd = CMD_NOP;
//  const char * CmdName = name;
//  if(cmd == CMD_NOP) CmdName = CmdNames[CMD_NOP];
//  if(cmd == CMD_ERR) CmdName = CmdNames[CMD_ERR];
//  if(errc > MAX_ERR) errc = MAX_ERR;
//  strcpy(LastError, CmdName);
//  strcat(LastError, " error: ");
//  strcat(LastError, ErrorMsg[errc]);
//  return(errc == ERR_NO);
//}

//-------------------------- Execute command: -------------------------------

int WakeCmdExe(unsigned char addr, struct wake_cmd cmd, unsigned char *real_rx, unsigned char *data)
{
  unsigned char rx, rx_addr, rx_cmd;
  if (wake_tx_frame(addr, cmd.code, cmd.need_tx, data)) return -1; //transmit frame
  if (wake_rx_frame(cmd.timeout, &rx_addr, &rx_cmd, &rx, data) == EXIT_FAILURE) return -2; //receive frame
  if (rx_addr != addr) return -3;
  if (rx_cmd != cmd.code) return -4;
  if (real_rx != 0) *real_rx = rx;
  if (cmd.need_rx == 0xff) return EXIT_SUCCESS; // no need real_rx control
  if (rx != cmd.need_rx) return -5;
  return EXIT_SUCCESS;
}

//---------------------------------------------------------------------------

