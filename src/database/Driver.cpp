#include "config.h"

#include <string>
#include <stdexcept>

#include <boost/algorithm/string/predicate.hpp>

#include "database/Driver.h"
#include "database/Query.h"
#ifdef ENABLE_MYSQL
#	include "database/driver/MySQL.h"
#endif

#include "misc.h"
#include "globals.h"


using namespace lotos2;
using database::Driver;


Driver* Driver::_instance=nullptr;

Driver* Driver::instance()
{
	if (!_instance) {
		std::string type=options.get("database.Type", "");
#ifdef ENABLE_MYSQL
		if (boost::iequals("mysql", type)) {
			_instance=new driver::MySQL;
			}
#endif
		}
	return _instance;
}

bool Driver::executeQuery(Query &query)
{
	return internalQuery(query.str());
}

bool Driver::executeQuery(const std::string &query)
{
	return internalQuery(query);
}

database::Result_ptr Driver::storeQuery(const std::string &query)
{
	return internalSelectQuery(query);
}

database::Result_ptr Driver::storeQuery(Query &query)
{
	return storeQuery(query.str());
}

void Driver::freeResult(Result *res)
{
	throw std::runtime_error("No database driver loaded, yet a Result was freed.");
}

database::Result_ptr Driver::verifyResult(Result_ptr result)
{
	if (!result->advance()) {
		return Result_ptr();
		}
	return result;
}
