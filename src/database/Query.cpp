#include "database/Query.h"

using namespace lotospp::database;

boost::recursive_mutex Query::database_lock;

Query::Query()
{
	database_lock.lock();
}

Query::~Query()
{
	database_lock.unlock();
}
