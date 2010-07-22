#include <QtCore>

#ifdef Q_OS_WIN32
int gettimeofday( struct timeval *tv, struct timezone *tz );
#endif
