#ifndef COMPORT_LIN_H
#define COMPORT_LIN_H

int portOpen(const char*);
void portSetOptions(long, char);
void portClose();
const char * portGetError();

int portWrite(unsigned char*, int);
int portRead(unsigned char *buf, int size, int timeout);

#endif
