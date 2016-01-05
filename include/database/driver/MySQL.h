#ifndef LOTOSPP_DATABASE_DRIVER_MYSQL_H
#define LOTOSPP_DATABASE_DRIVER_MYSQL_H


#include "config.h"

#ifdef ENABLE_MYSQL

#include "system/system.h"

#include <mysql.h>

#include <cstdint>
#include <string>
#include <map>

#include "database/Driver.h"
#include "database/Result.h"


namespace lotospp {
	namespace database {
		namespace driver {

class MySQL
	: public lotospp::database::Driver
{
public:
	MySQL();
	virtual ~MySQL();

	virtual bool getParam(lotospp::database::DBParam_t param);

	virtual bool beginTransaction();
	virtual bool rollback();
	virtual bool commit();

	virtual uint64_t getLastInsertedRowID();

	virtual std::string escapeString(const std::string &s);
	virtual std::string escapeBlob(const char* s, uint32_t length);

protected:
	virtual bool internalQuery(const std::string &query);
	virtual lotospp::database::Result_ptr internalSelectQuery(const std::string &query);
	virtual void freeResult(lotospp::database::Result *res);

	MYSQL m_handle;
};

class MySQLResult
	: public lotospp::database::Result
{
	friend class MySQL;

public:
	virtual int32_t getDataInt(const std::string &s);
	virtual uint32_t getDataUInt(const std::string &s);
	virtual int64_t getDataLong(const std::string &s);
	virtual std::string getDataString(const std::string &s);
	virtual const char* getDataStream(const std::string &s, unsigned long &size);

	virtual lotospp::database::Result_ptr advance();
	virtual bool empty();

protected:
	MySQLResult(MYSQL_RES* res);
	virtual ~MySQLResult();

	typedef std::map<const std::string, uint32_t> listNames_t;
	listNames_t m_listNames;

	MYSQL_RES* m_handle;
	MYSQL_ROW m_row;
};

		} // namespace driver
	} // namespace database
} // namespace lotospp

#endif // ENABLE_MYSQL
#endif // LOTOSPP_DATABASE_DRIVER_MYSQL_H
