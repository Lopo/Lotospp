#ifndef LOTOSPP_DATABASE_DRIVERS_MYSQL_H
#define LOTOSPP_DATABASE_DRIVERS_MYSQL_H


#include "config.h"

#ifdef WITH_MYSQL

#include "../Driver.h"
#include "../Result.h"
#include <string>
#include <map>
#include <cstdint>
#ifdef OS_WIN
#	include <winsock.h>
#endif
#include <mysql.h>


namespace LotosPP::Database::Drivers {

class MySQL
	: public LotosPP::Database::Driver
{
public:
	MySQL();
	virtual ~MySQL();

	virtual bool getParam(const LotosPP::Database::DBParam_t& param);

	virtual bool beginTransaction();
	virtual bool rollback();
	virtual bool commit();

	virtual uint64_t getLastInsertedRowID();

	virtual std::string escapeString(const std::string& s);
	virtual std::string escapeBlob(const char* s, uint32_t length);

protected:
	virtual bool internalQuery(const std::string& query);
	virtual LotosPP::Database::Result_ptr internalSelectQuery(const std::string& query);
	virtual void freeResult(LotosPP::Database::Result* res);

	MYSQL m_handle;
};

class MySQLResult
	: public LotosPP::Database::Result
{
	friend class MySQL;

public:
	virtual int32_t getDataInt(const std::string& s);
	virtual uint32_t getDataUInt(const std::string& s);
	virtual int64_t getDataLong(const std::string& s);
	virtual std::string getDataString(const std::string& s);
	virtual const char* getDataStream(const std::string& s, unsigned long& size);

	virtual LotosPP::Database::Result_ptr advance();
	virtual bool empty();

protected:
	MySQLResult(MYSQL_RES* res);
	virtual ~MySQLResult();

	typedef std::map<const std::string, uint32_t> listNames_t;
	listNames_t m_listNames{};

	MYSQL_RES* m_handle{nullptr};
	MYSQL_ROW m_row;
};

	}

#endif // WITH_MYSQL
#endif
