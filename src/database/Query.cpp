#include "config.h"

#include "database/Query.h"


boost::recursive_mutex Query::database_lock;

Query::Query()
{
	database_lock.lock();
}

Query::~Query()
{
	database_lock.unlock();
}
