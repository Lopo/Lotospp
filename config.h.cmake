#ifndef CONFIG_H
#define CONFIG_H 1

#define PACKAGE "Lotos2"

#cmakedefine HAVE_DIRECT_H
#ifndef getcwd
#	ifdef HAVE_DIRECT_H
#		include <direct.h>
#		define getcwd _getcwd
#	endif
#endif

#cmakedefine HAVE_FORK 1
#cmakedefine HAVE_SNPRINTF 1
#cmakedefine HAVE_VSNPRINTF 1


// Enable secure communication channels
#cmakedefine USE_OPENSSL 1
// Enable IPv6 support
#cmakedefine USE_IPV6 1

/* define if you use win32 thread */
#cmakedefine USE_MSTHREADS
/* define if you use pthreads */
#cmakedefine USE_PTHREADS

#endif
