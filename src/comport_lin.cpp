#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#include "comport_lin.h"

//#define DEVICE "/dev/ttyS%d"

static int fd;
static struct termios com_oldtio,com_newtio;
static char* error;

int portOpen (const char *portName)
{
  fd=::open(portName,O_RDWR | O_NOCTTY);
  if (fd<0) { error=strerror(errno); return EXIT_FAILURE; }
  tcgetattr(fd,&com_oldtio); /* save current port settings */
  return EXIT_SUCCESS;
}

void portSetOptions(long rate,char par)
{
  long baud;

  switch (rate)
  {
    case   1200: baud=B1200;   break;
    case   4800: baud=B4800;   break;
    case   9600: baud=B9600;   break;
    case  19200: baud=B19200;  break;
    case  38400: baud=B38400;  break;
    case  57600: baud=B57600;  break;
    case 115200: baud=B115200; break;
    default: baud=B9600; break;
  };

  bzero(&com_newtio,sizeof(com_newtio));
  com_newtio.c_cflag=baud | CS8 | CLOCAL | CREAD;
  com_newtio.c_oflag=0;
  com_newtio.c_lflag=0;     /* set input mode (non-canonical, no echo,...) */
  com_newtio.c_cc[VTIME]=0; /* inter-character timer unused */
  com_newtio.c_cc[VMIN]=0;  /* blocking read until 0 chars received */

  switch (par)
  {
    case   0:   com_newtio.c_cflag &= ~PARENB;	// no parity control
                com_newtio.c_iflag = IGNPAR;
                break;
    case   1:   com_newtio.c_iflag = (INPCK | ISTRIP);
                com_newtio.c_cflag |= PARENB;	// odd
                com_newtio.c_cflag |= PARODD;
                break;
    case   2:   break;
                com_newtio.c_iflag = (INPCK | ISTRIP);
                com_newtio.c_cflag |= PARENB;	// event
                com_newtio.c_cflag &= ~PARODD;
                break;
    default:    com_newtio.c_cflag &= ~PARENB; break;
  };

  tcflush(fd,TCIFLUSH);
  tcsetattr(fd,TCSANOW,&com_newtio);
}


int portWrite(unsigned char *buf, int size)
{
  int result = write(fd,buf,size);
  tcflush(fd,TCIFLUSH);
  return result;
}

int portRead(unsigned char *buf, int size, int timeout)
{
  fd_set fds;
  struct timeval tv;
  int rt, sz;

  FD_ZERO(&fds);
  FD_SET(fd,&fds);

  tv.tv_sec=timeout;
  tv.tv_usec=0;

  rt=select(fd+1,&fds,NULL,NULL,&tv);
  if (rt)
  {
    sz=read(fd,buf,size);
    return sz;
  }
  return 0;
}

void portClose()
{
  tcsetattr(fd,TCSANOW,&com_oldtio);
  close(fd);
}

const char * portGetError()
{
  return error;
}

