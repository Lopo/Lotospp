/*
 * File:   globals.h
 * Author: lopo
 *
 * Created on August 17, 2013, 9:29 PM
 */

#ifndef GLOBALS_H
#define GLOBALS_H

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

#endif /* GLOBALS_H */
