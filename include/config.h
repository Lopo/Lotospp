#ifndef CONFIG_H
#define CONFIG_H 1

#define PACKAGE "Lotos2"

/* #undef HAVE_DIRECT_H */
#ifndef getcwd
#	ifdef HAVE_DIRECT_H
#		include <direct.h>
#		define getcwd _getcwd
#	endif
#endif

/* #undef HAVE_FORK */
/* #undef HAVE_SNPRINTF */
/* #undef HAVE_VSNPRINTF */


// Enable secure communication channels
/* #undef USE_OPENSSL */
// Enable IPv6 support
#define USE_IPV6 1

/* define if you use win32 thread */
/* #undef USE_MSTHREADS */
/* define if you use pthreads */
#define USE_PTHREADS

#endif
