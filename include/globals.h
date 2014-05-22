#ifndef LOTOS2_GLOBALS_H
#define LOTOS2_GLOBALS_H

#include <time.h>
#include <boost/property_tree/ptree.hpp>
#include <pthread.h>

#include "Scheduler.h"
#include "Talker.h"

#ifndef MAINFILE
#define EXTERN extern
#else
#define EXTERN
#endif


EXTERN boost::property_tree::ptree options;
EXTERN struct std::tm serverTimeTms;

EXTERN pthread_mutex_t log_mutex;

EXTERN Scheduler g_scheduler;
EXTERN Dispatcher g_dispatcher;
EXTERN Talker g_talker;

#endif /* LOTOS2_GLOBALS_H */
