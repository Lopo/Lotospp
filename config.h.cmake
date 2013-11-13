#ifndef LOTOS2_CONFIG_H
#define LOTOS2_CONFIG_H 1

#define PACKAGE "Lotos2"
#define LOTOS2_VERSION "0.0.2"
#define LOTOS2_NAME "Lotos2"

#cmakedefine HAVE_DIRECT_H
#ifndef getcwd
#	ifdef HAVE_DIRECT_H
#		include <direct.h>
#		define getcwd _getcwd
#	endif
#endif

#cmakedefine HAVE_FORK 1
//#cmakedefine HAVE_SNPRINTF 1
//#cmakedefine HAVE_VSNPRINTF 1


// Enable secure communication channels
//#cmakedefine USE_OPENSSL 1
// Use MD5 from OpenSSL
//#cmakedefine HAVE_OPENSSL_MD5_H
// Enable IPv6 support
#cmakedefine USE_IPV6 1


/* define if you use win32 thread */
#cmakedefine USE_MSTHREADS 1
/* define if you use pthreads */
#cmakedefine USE_PTHREADS 1

#endif /* LOTOS2_CONFIG_H */
