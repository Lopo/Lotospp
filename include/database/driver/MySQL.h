#ifndef LOTOS2_DATABASE_DRIVER_MYSQL_H
#define LOTOS2_DATABASE_DRIVER_MYSQL_H


#include "config.h"

#ifdef ENABLE_MYSQL

#ifdef __MYSQL_ALT_INCLUDE__
#	include <mysql.h>
#else
#	include <mysql/mysql.h>
#endif

#include <cstdint>
#include <string>
#include <map>

#include "database/Driver.h"
#include "database/Result.h"


namespace lotos2 {
	namespace database {
		namespace driver {

class MySQL
	: public Driver
{
public:
	MySQL();
	virtual ~MySQL();

	virtual bool getParam(lotos2::database::DBParam_t param);

	virtual bool beginTransaction();
	virtual bool rollback();
	virtual bool commit();

	virtual uint64_t getLastInsertedRowID();

	virtual std::string escapeString(const std::string &s);
	virtual std::string escapeBlob(const char* s, uint32_t length);

protected:
	virtual bool internalQuery(const std::string &query);
	virtual Result_ptr internalSelectQuery(const std::string &query);
	virtual void freeResult(Result *res);

	MYSQL m_handle;
};

class MySQLResult
	: public Result
{
	friend class MySQL;

public:
	virtual int32_t getDataInt(const std::string &s);
	virtual uint32_t getDataUInt(const std::string &s);
	virtual int64_t getDataLong(const std::string &s);
	virtual std::string getDataString(const std::string &s);
	virtual const char* getDataStream(const std::string &s, unsigned long &size);

	virtual Result_ptr advance();
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
} // namespace lotos2

#endif // ENABLE_MYSQL
#endif // LOTOS2_DATABASE_DRIVER_MYSQL_H
