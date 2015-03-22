#ifndef LOTOS2_GLOBALS_H
#define LOTOS2_GLOBALS_H


#include "config.h"

#include <ctime>

#include <boost/property_tree/ptree.hpp>

#include "Scheduler.h"
#include "Dispatcher.h"
#include "Talker.h"

#ifndef MAINFILE
#define EXTERN extern
#else
#define EXTERN
#endif


namespace lotos2 {

EXTERN boost::property_tree::ptree options;
EXTERN struct std::tm serverTimeTms;

EXTERN Scheduler g_scheduler;
EXTERN Dispatcher g_dispatcher;
EXTERN Talker g_talker;

} // namespace lotos2

#endif // LOTOS2_GLOBALS_H
