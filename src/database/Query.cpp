#include "config.h"

#include "database/Query.h"

using namespace lotos2;
using lotos2::database::Query;

boost::recursive_mutex Query::database_lock;

Query::Query()
{
	database_lock.lock();
}

Query::~Query()
{
	database_lock.unlock();
}
