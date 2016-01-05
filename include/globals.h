#ifndef LOTOSPP_GLOBALS_H
#define LOTOSPP_GLOBALS_H


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


namespace lotospp {

EXTERN boost::property_tree::ptree options;
EXTERN struct std::tm serverTimeTms;

EXTERN Scheduler g_scheduler;
EXTERN Dispatcher g_dispatcher;
EXTERN Talker g_talker;

} // namespace lotospp

#endif // LOTOSPP_GLOBALS_H
