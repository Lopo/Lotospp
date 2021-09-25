#include "database/Query.h"

using namespace lotospp::database;


Query::Query()
{
	database_lock.lock();
}

Query::~Query()
{
	database_lock.unlock();
}
