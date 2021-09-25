#include "Query.h"

using namespace LotosPP::Database;


Query::Query()
{
	database_lock.lock();
}

Query::~Query()
{
	database_lock.unlock();
}
