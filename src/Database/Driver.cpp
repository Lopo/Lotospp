#include "Driver.h"
#include "Query.h"
#ifdef WITH_MYSQL
#	include "Drivers/MySQL.h"
#endif
#include "Strings/misc.h"
#include "globals.h"
#include <boost/algorithm/string/predicate.hpp>
#include <stdexcept>


using namespace LotosPP::Database;


Driver* Driver::instance()
{
	if (!_instance) {
		std::string type=LotosPP::options.get("database.Type", "");
#ifdef WITH_MYSQL
		if (boost::iequals("mysql", type)) {
			_instance=new Drivers::MySQL;
			}
#endif
		}
	return _instance;
}

bool Driver::executeQuery(Query& query)
{
	return internalQuery(query.str());
}

bool Driver::executeQuery(const std::string& query)
{
	return internalQuery(query);
}

Result_ptr Driver::storeQuery(const std::string& query)
{
	return internalSelectQuery(query);
}

Result_ptr Driver::storeQuery(Query& query)
{
	return storeQuery(query.str());
}

void Driver::freeResult([[maybe_unused]]Result* res)
{
	throw std::runtime_error("No database driver loaded, yet a Result was freed.");
}

Result_ptr Driver::verifyResult(Result_ptr result)
{
	if (!result->advance()) {
		return Result_ptr();
		}
	return result;
}
