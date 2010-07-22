#include "utils.h"
#ifdef Q_OS_WIN32

#include <windows.h>
#include <time.h>

int gettimeofday( struct timeval *tv, struct timezone *tz )
{
  time_t rawtime;

  time(&rawtime);
  //tv->tv_sec = (long)rawtime;

  // here starts the microsecond resolution:

  LARGE_INTEGER tickPerSecond;
  LARGE_INTEGER tick; // a point in time

  // get the high resolution counter's accuracy
  QueryPerformanceFrequency(&tickPerSecond);

  // what time is it ?
  QueryPerformanceCounter(&tick);

  // and here we get the current microsecond! \o/
  tv->tv_usec = (tick.QuadPart % tickPerSecond.QuadPart);
  tv->tv_sec = (long)rawtime;

  return 0;
}
#endif // Q_OS_WIN32


//_LARGE_INTEGER Pt, Pt0;
//QueryPerformanceCounter(&Pt0);
//QueryPerformanceCounter(&Pt);
//ExeTime = (Pt.LowPart-Pt0.LowPart)*Pms;
//Format("Command(s) execution time, ms: %6.3f",ARRAYOFCONST((ExeTime)));
