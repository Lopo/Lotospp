#include "config.h"

#include "database/Driver.h"

#ifdef USE_MYSQL
#include "database/driver/MySQL.h"
#endif

#include "globals.h"
#include "misc.h"


Driver* Driver::_instance=NULL;

Driver* Driver::instance()
{

	if (!_instance) {
		std::string type=options.get<std::string>("global.sqlType");
		::toLowerCaseString(type);
#ifdef USE_MYSQL
		if (type=="mysql") {
			_instance=new MySQL;
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

Result_ptr Driver::storeQuery(const std::string &query)
{
	return internalSelectQuery(query);
}

Result_ptr Driver::storeQuery(Query &query)
{
	return storeQuery(query.str());
}

void Driver::freeResult(Result *res)
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
