#include "config.h"
#ifdef ENABLE_MYSQL

#ifdef __MYSQL_ALT_INCLUDE__
#	include "errmsg.h"
#else
#	include <mysql/errmsg.h>
#endif

#include <iostream>
#include <string>

#include <boost/bind.hpp>

#include "database/driver/MySQL.h"
#include "database/Query.h"

#include "globals.h"


using namespace lotos2;
using namespace lotos2::database::driver;

/** MySQL definitions */

MySQL::MySQL()
{
	// connection handle initialization
	if (!mysql_init(&m_handle)) {
		std::cout << std::endl << "Failed to initialize MySQL connection handle." << std::endl;
		return;
		}

	// automatic reconnect
	my_bool reconnect=true;
	mysql_options(&m_handle, MYSQL_OPT_RECONNECT, &reconnect);

	// connects to database
	if (!mysql_real_connect(&m_handle,
			options.get<char*>("database.Host", "localhost"),
			options.get<char*>("database.User", "root"),
			options.get<char*>("database.Pass", ""),
			options.get<char*>("database.Db", "lotos2"),
			options.get<unsigned int>("database.Port", 3306),
			NULL, 0)
		) {
		std::cout << "Failed to connect to database. MYSQL ERROR: " << mysql_error(&m_handle) << std::endl;
		return;
		}

	if (MYSQL_VERSION_ID<50019) {
		//mySQL servers < 5.0.19 has a bug where MYSQL_OPT_RECONNECT is (incorrectly) reset by mysql_real_connect calls
		//See http://dev.mysql.com/doc/refman/5.0/en/mysql-options.html for more information.
		mysql_options(&m_handle, MYSQL_OPT_RECONNECT, &reconnect);
		std::cout << std::endl << "[Warning] Outdated mySQL server detected. Consider upgrading to a newer version." << std::endl;
		}

	m_connected=true;

	if (options.get<std::string>("global.mapStorageType", "")=="binary") {
		Query query;
		query << "SHOW variables LIKE 'max_allowed_packet';";

		Result_ptr result;
		if ((result=storeQuery(query.str()))) {
			int32_t max_query=result->getDataInt("Value");

			if (max_query<16777216) {
				std::cout << std::endl << "[Warning] max_allowed_packet might be set too low for binary map storage." << std::endl;
				std::cout << "Use the following query to raise max_allow_packet: " << std::endl;
				std::cout << "\tSET GLOBAL max_allowed_packet = 16777216;" << std::endl;
				}
			}
		}
}

MySQL::~MySQL()
{
	mysql_close(&m_handle);
}

bool MySQL::getParam(database::DBParam_t param)
{
	switch (param) {
		case DBPARAM_MULTIINSERT:
			return true;
		default:
			return false;
		}
}

bool MySQL::beginTransaction()
{
	return executeQuery("BEGIN");
}

bool MySQL::rollback()
{
	if (!m_connected) {
		return false;
		}

#ifdef __DEBUG_SQL__
	std::cout << "ROLLBACK" << std::endl;
#endif

	if (mysql_rollback(&m_handle)!=0) {
		std::cout << "mysql_rollback(): MYSQL ERROR: " << mysql_error(&m_handle) << std::endl;
		return false;
		}

	return true;
}

bool MySQL::commit()
{
	if (!m_connected) {
		return false;
		}

#ifdef __DEBUG_SQL__
	std::cout << "COMMIT" << std::endl;
#endif
	if (mysql_commit(&m_handle)!=0) {
		std::cout << "mysql_commit(): MYSQL ERROR: " << mysql_error(&m_handle) << std::endl;
		return false;
		}

	return true;
}

bool MySQL::internalQuery(const std::string &query)
{
	if (!m_connected) {
		return false;
		}

#ifdef __DEBUG_SQL__
	std::cout << "MYSQL QUERY: " << query << std::endl;
#endif

	bool state=true;

	// executes the query
	if (mysql_real_query(&m_handle, query.c_str(), query.length())!=0) {
		std::cout << "mysql_real_query(): " << query.substr(0, 256) << ": MYSQL ERROR: " << mysql_error(&m_handle) << std::endl;
		int error=mysql_errno(&m_handle);

		if (error==CR_SERVER_LOST || error==CR_SERVER_GONE_ERROR) {
			m_connected=false;
			}

		state=false;
		}

	// we should call that every time as someone would call executeQuery('SELECT...')
	// as it is described in MySQL manual: "it doesn't hurt" :P
	MYSQL_RES* m_res=mysql_store_result(&m_handle);

	if (m_res) {
		mysql_free_result(m_res);
		}

	return state;
}

database::Result_ptr MySQL::internalSelectQuery(const std::string &query)
{
	if (!m_connected) {
		return Result_ptr();
		}

#ifdef __DEBUG_SQL__
	std::cout << "MYSQL QUERY: " << query << std::endl;
#endif

	// executes the query
	if (mysql_real_query(&m_handle, query.c_str(), query.length())!=0) {
		std::cout << "mysql_real_query(): " << query << ": MYSQL ERROR: " << mysql_error(&m_handle) << std::endl;
		int error=mysql_errno(&m_handle);

		if (error==CR_SERVER_LOST || error==CR_SERVER_GONE_ERROR) {
			m_connected=false;
			}
		}

	// we should call that every time as someone would call executeQuery('SELECT...')
	// as it is described in MySQL manual: "it doesn't hurt" :P
	MYSQL_RES* m_res=mysql_store_result(&m_handle);

	// error occured
	if (!m_res) {
		std::cout << "mysql_store_result(): " << query.substr(0, 256) << ": MYSQL ERROR: " << mysql_error(&m_handle) << std::endl;
		int error=mysql_errno(&m_handle);

		if (error==CR_SERVER_LOST || error==CR_SERVER_GONE_ERROR) {
			m_connected=false;
			}

		return Result_ptr();
		}

	// retriving results of query
	Result_ptr res(new MySQLResult(m_res), boost::bind(&Driver::freeResult, this, _1));
	return verifyResult(res);
}

uint64_t MySQL::getLastInsertedRowID()
{
	return (uint64_t)mysql_insert_id(&m_handle);
}

std::string MySQL::escapeString(const std::string &s)
{
	return escapeBlob(s.c_str(), s.length());
}

std::string MySQL::escapeBlob(const char* s, uint32_t length)
{
	// remember about quoiting even an empty string!
	if (!s) {
		return std::string("''");
		}

	// the worst case is 2n + 1
	char* output=new char[length*2+1];

	// quotes escaped string and frees temporary buffer
	mysql_real_escape_string(&m_handle, output, s, length);
	std::string r="'";
	r+=output;
	r+="'";
	delete[] output;
	return r;
}

void MySQL::freeResult(Result* res)
{
	delete (MySQLResult*)res;
}

/** MySQLResult definitions */

int32_t MySQLResult::getDataInt(const std::string &s)
{
	listNames_t::iterator it=m_listNames.find(s);
	if (it!=m_listNames.end()) {
		if (m_row[it->second]==NULL) {
			return 0;
			}
		return atoi(m_row[it->second]);
		}

	std::cout << "Error during getDataInt(" << s << ")." << std::endl;
	return 0; // Failed
}

uint32_t MySQLResult::getDataUInt(const std::string &s)
{
	listNames_t::iterator it=m_listNames.find(s);
	if (it!=m_listNames.end()) {
		if (m_row[it->second]==NULL) {
			return 0;
			}
		std::istringstream os(m_row[it->second]);
		uint32_t res;
		os >> res;
		return res;
		}

	std::cout << "Error during getDataInt(" << s << ")." << std::endl;
	return 0; // Failed
}

int64_t MySQLResult::getDataLong(const std::string &s)
{
	listNames_t::iterator it=m_listNames.find(s);
	if (it!=m_listNames.end()) {
		if (m_row[it->second]==NULL) {
			return 0;
			}
		return atoll(m_row[it->second]);
		}

	std::cout << "Error during getDataLong(" << s << ")." << std::endl;
	return 0; // Failed
}

std::string MySQLResult::getDataString(const std::string &s)
{
	listNames_t::iterator it=m_listNames.find(s);
	if (it!=m_listNames.end()) {
		return m_row[it->second]==NULL
			? std::string("")
			: std::string(m_row[it->second]);
		}

	std::cout << "Error during getDataString(" << s << ")." << std::endl;
	return std::string(""); // Failed
}

const char* MySQLResult::getDataStream(const std::string &s, unsigned long &size)
{
	listNames_t::iterator it=m_listNames.find(s);
	if (it!=m_listNames.end()) {
		if (m_row[it->second]==NULL) {
			size=0;
			return NULL;
			}
		size=mysql_fetch_lengths(m_handle)[it->second];
		return m_row[it->second];
		}

	std::cout << "Error during getDataStream(" << s << ")." << std::endl;
	size=0;
	return NULL;
}

database::Result_ptr MySQLResult::advance()
{
	m_row=mysql_fetch_row(m_handle);
	return m_row!=NULL
		? shared_from_this()
		: Result_ptr();
}

bool MySQLResult::empty()
{
	return m_row==NULL;
}

MySQLResult::MySQLResult(MYSQL_RES* res)
{
	m_handle=res;
	m_listNames.clear();

	MYSQL_FIELD* field;
	int32_t i=0;
	while ((field=mysql_fetch_field(m_handle))) {
		m_listNames[field->name]=i;
		i++;
		}
}

MySQLResult::~MySQLResult()
{
	mysql_free_result(m_handle);
}

#endif // ENABLE_MYSQL
