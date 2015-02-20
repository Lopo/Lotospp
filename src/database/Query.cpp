#include "config.h"

#include <boost/thread/recursive_mutex.hpp>

#include "database/Query.h"

using namespace lotos2;
using database::Query;

boost::recursive_mutex Query::database_lock;

Query::Query()
{
	database_lock.lock();
}

Query::~Query()
{
	database_lock.unlock();
}
