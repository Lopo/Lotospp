#ifndef LOTOSPP_DATABASE_DRIVER_H
#define	LOTOSPP_DATABASE_DRIVER_H


#include "config.h"
#include <boost/shared_ptr.hpp>
#include <string>
#include <cstdint>


namespace LotosPP::Database {
	class Query;
	class Result;
	typedef boost::shared_ptr<Result> Result_ptr;

enum DBParam_t {
	DBPARAM_MULTIINSERT=1
	};

class Driver
{
public:
	/**
	 * Singleton implementation
	 *
	 * Returns instance of database handler. Don't create database (or drivers) instances in your code - instead of it use Database::instance(). This method stores static instance of connection class internaly to make sure exacly one instance of connection is created for entire system.
	 *
	 * @return database connection handler singleton
	 */
	static Driver* instance();

	/**
	 * Database information
	 *
	 * Returns currently used database attribute
	 *
	 * @param param parameter to get
	 * @return suitable for given parameter
	 */
	virtual bool getParam(const DBParam_t& param)
	{
		return false;
	};

	/**
	 * Database connected
	 *
	 * Returns whether or not the database is connected
	 *
	 * @return whether or not the database is connected
	 */
	bool isConnected() const
	{
		return m_connected;
	};

protected:
	/**
	 * Transaction related methods
	 *
	 * Methods for starting, committing and rolling back transaction. Each of the returns boolean value.
	 *
	 * @return true on success, false on error
	 * @note If your database system doesn't support transactions you should return true - it's not feature test, code should work without transaction, just will lack integrity.
	 */
	friend class Transaction;
	virtual bool beginTransaction()=0;
	virtual bool rollback()=0;
	virtual bool commit()=0;

public:
	/**
	 * Executes command
	 *
	 * Executes query which doesn't generates results (eg. INSERT, UPDATE, DELETE...)
	 *
	 * @param query query command
	 * @return true on success, false on error
	 */
	bool executeQuery(const std::string& query);
	bool executeQuery(Query& query);

	/**
	 * Returns ID of last inserted row
	 *
	 * @return id of last inserted row, 0 if last query did not result in any rows with auto_increment keys
	 */
	virtual uint64_t getLastInsertedRowID()=0;

	/**
	 * Queries database
	 *
	 * Executes query which generates results (mostly SELECT)
	 *
	 * @param query
	 * @return results object (null on error)
	 */
	Result_ptr storeQuery(const std::string& query);
	Result_ptr storeQuery(Query& query);

	/**
	 * Escapes string for query
	 *
	 * Prepares string to fit SQL queries including quoting it
	 *
	 * @param s string to be escaped
	 * @return quoted string
	 */
	virtual std::string escapeString(const std::string& s)=0;
	/**
	 * Escapes binary stream for query.
	 *
	 * Prepares binary stream to fit SQL queries.
	 *
	 * @param s binary stream
	 * @param length stream length
	 * @return quoted string
	 */
	virtual std::string escapeBlob(const char* s, uint32_t length)=0;

	/**
	 * Resource freeing
	 * Used as argument to shared_ptr, you need not call this directly
	 *
	 * @param res resource to be freed
	 */
	virtual void freeResult(Result* res);

protected:
	Driver()
	{};
	virtual ~Driver()
	{};

	/**
	 * Executes a query directly
	 */
	virtual bool internalQuery(const std::string& query)=0;
	virtual Result_ptr internalSelectQuery(const std::string& query)=0;

	Result_ptr verifyResult(Result_ptr result);

	bool m_connected{false};

private:
	static inline Driver* _instance{nullptr};
};

	}

#endif
