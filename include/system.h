#ifndef __LOTOS2_SYSTEM_H__
#define __LOTOS2_SYSTEM_H__

#include "config.h"

#include <stdint.h>

#ifdef __WINDOWS__
#ifdef __WIN_LOW_FRAG_HEAP__
#define _WIN32_WINNT 0x0501
#endif
#include <winsock2.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/timeb.h>

inline int64_t SYS_TIME()
{
  _timeb t;
  _ftime(&t);
  return ((int64_t)t.millitm)+((int64_t)t.time)*1000;
};

typedef int socklen_t;

#else  // #if defined __WINDOWS__

#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

inline int64_t SYS_TIME()
{
	timeb t;
	ftime(&t);
	return ((int64_t)t.millitm)+((int64_t)t.time)*1000;
};

#ifndef SOCKET
#define SOCKET int
#endif

#ifndef closesocket
#define closesocket close
#endif

#endif // #if defined __WINDOWS__

#endif // #ifndef __LOTOS2_SYSTEM_H__
