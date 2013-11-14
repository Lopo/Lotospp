#ifndef LOTOS2_GLOBALS_H
#define LOTOS2_GLOBALS_H

#include <time.h>
#include <boost/property_tree/ptree.hpp>
#include <pthread.h>

#ifndef MAINFILE
#define EXTERN extern
#else
#define EXTERN
#endif


EXTERN boost::property_tree::ptree options;
EXTERN struct std::tm serverTimeTms;

EXTERN pthread_mutex_t log_mutex;

#endif /* LOTOS2_GLOBALS_H */
