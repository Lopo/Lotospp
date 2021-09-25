#ifndef LOTOSPP_GLOBALS_H
#define LOTOSPP_GLOBALS_H

#include "config.h"
#include "Common/Scheduler.h"
#include "Common/Dispatcher.h"
#include "Common/Talker.h"
#include <boost/property_tree/ptree.hpp>
#include <ctime>

#ifndef MAINFILE
#	define EXTERN extern
#else
#	define EXTERN
#endif


namespace LotosPP {

EXTERN boost::property_tree::ptree options;
EXTERN struct std::tm serverTimeTms;

EXTERN Common::Scheduler g_scheduler;
EXTERN Common::Dispatcher g_dispatcher;
EXTERN Common::Talker g_talker;

	}

#endif
