#ifndef LOTOSPP_SYSTEM_H
#define LOTOSPP_SYSTEM_H


#include "config.h"

#include <sys/timeb.h>

#include <cstdint>

#if defined WIN32 || defined __WINDOWS__
#define NOGDI
#include <winsock2.h>
#include <Windows.h>
#include <stddef.h>
#include <stdlib.h>

inline int64_t SYS_TIME()
{
  _timeb t;
  _ftime(&t);
  return ((int64_t)t.millitm)+((int64_t)t.time)*1000;
};

typedef int socklen_t;

#else  // #if defined WIN32 || defined __WINDOWS__

//#include <unistd.h>
//#include <time.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netdb.h>
//#include <errno.h>

inline int64_t SYS_TIME()
{
	timeb t;
	ftime(&t);
	return ((int64_t)t.millitm)+((int64_t)t.time)*1000;
};

#ifndef SOCKET
#	define SOCKET int
#endif

#ifndef closesocket
#	define closesocket close
#endif

#endif // #if defined WIN32 || defined __WINDOWS__

#endif // LOTOSPP_SYSTEM_H
