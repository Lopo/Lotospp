#include "MySQL.h"

#include "../Query.h"
#include "globals.h"
//#include <boost/bind/bind.hpp>
#include <iostream>
#include <errmsg.h> // mysql


using namespace LotosPP::Database::Drivers;
using namespace std;

/** MySQL definitions */

MySQL::MySQL()
{
	// connection handle initialization
	if (!mysql_init(&m_handle)) {
		cout << endl << "Failed to initialize MySQL connection handle." << endl;
		return;
		}

	// automatic reconnect
	my_bool reconnect{true};
	mysql_options(&m_handle, MYSQL_OPT_RECONNECT, &reconnect);

	using LotosPP::options;
	// connects to database
	if (!mysql_real_connect(&m_handle,
			options.get("database.Host", "localhost").c_str(),
			options.get("database.User", "root").c_str(),
			options.get("database.Pass", "").c_str(),
			options.get("database.Db", "lotos").c_str(),
			options.get<unsigned int>("database.Port", MYSQL_PORT),
			NULL,
			0
			)
		) {
		cout << "Failed to connect to database. MYSQL ERROR: " << mysql_error(&m_handle) << endl;
		return;
		}

	if (MYSQL_VERSION_ID<50019) {
		//mySQL servers < 5.0.19 has a bug where MYSQL_OPT_RECONNECT is (incorrectly) reset by mysql_real_connect calls
		//See http://dev.mysql.com/doc/refman/5.0/en/mysql-options.html for more information.
		mysql_options(&m_handle, MYSQL_OPT_RECONNECT, &reconnect);
		cout << endl << "[Warning] Outdated MySQL server detected (" << MYSQL_SERVER_VERSION << "). Consider upgrading to a newer version." << endl;
		}

	m_connected=true;

	if (options.get<string>("global.mapStorageType", "")=="binary") {
		LotosPP::Database::Query query;
		query << "SHOW variables LIKE 'max_allowed_packet';";

		if (LotosPP::Database::Result_ptr result=storeQuery(query.str()); result && result->getDataInt("Value")<16777216) {
			cout << endl << "[Warning] max_allowed_packet might be set too low for binary map storage." << endl;
			cout << "Use the following query to raise max_allow_packet: " << endl;
			cout << "\tSET GLOBAL max_allowed_packet = 16777216;" << endl;
			}
		}
}

MySQL::~MySQL()
{
	mysql_close(&m_handle);
}

bool MySQL::getParam(const LotosPP::Database::DBParam_t& param) const
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
	cout << "ROLLBACK" << endl;
#endif

	if (mysql_rollback(&m_handle)) {
		cout << "mysql_rollback(): MYSQL ERROR: " << mysql_error(&m_handle) << endl;
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
	cout << "COMMIT" << endl;
#endif
	if (mysql_commit(&m_handle)) {
		cout << "mysql_commit(): MYSQL ERROR: " << mysql_error(&m_handle) << endl;
		return false;
		}

	return true;
}

bool MySQL::internalQuery(const string& query)
{
	if (!m_connected) {
		return false;
		}

#ifdef __DEBUG_SQL__
	cout << "MYSQL QUERY: " << query << endl;
#endif

	bool state{true};

	// executes the query
	if (mysql_real_query(&m_handle, query.c_str(), query.length())) {
		cout << "mysql_real_query(): " << query.substr(0, 256) << ": MYSQL ERROR: " << mysql_error(&m_handle) << endl;
		if (int error=mysql_errno(&m_handle); error==CR_SERVER_LOST || error==CR_SERVER_GONE_ERROR) {
			m_connected=false;
			}

		state=false;
		}

	// we should call that every time as someone would call executeQuery('SELECT...')
	// as it is described in MySQL manual: "it doesn't hurt" :P
	if (MYSQL_RES* m_res=mysql_store_result(&m_handle); m_res) {
		mysql_free_result(m_res);
		}

	return state;
}

LotosPP::Database::Result_ptr MySQL::internalSelectQuery(const std::string& query)
{
	if (!m_connected) {
		return LotosPP::Database::Result_ptr();
		}

#ifdef __DEBUG_SQL__
	cout << "MYSQL QUERY: " << query << endl;
#endif

	// executes the query
	if (mysql_real_query(&m_handle, query.c_str(), query.length())) {
		cout << "mysql_real_query(): " << query << ": MYSQL ERROR: " << mysql_error(&m_handle) << endl;
		if (int error=mysql_errno(&m_handle); error==CR_SERVER_LOST || error==CR_SERVER_GONE_ERROR) {
			m_connected=false;
			}
		}

	// we should call that every time as someone would call executeQuery('SELECT...')
	// as it is described in MySQL manual: "it doesn't hurt" :P
	MYSQL_RES* m_res=mysql_store_result(&m_handle);
	// error occured
	if (!m_res) {
		cout << "mysql_store_result(): " << query.substr(0, 256) << ": MYSQL ERROR: " << mysql_error(&m_handle) << endl;
		if (int error=mysql_errno(&m_handle); error==CR_SERVER_LOST || error==CR_SERVER_GONE_ERROR) {
			m_connected=false;
			}

		return LotosPP::Database::Result_ptr();
		}

	// retriving results of query
	LotosPP::Database::Result_ptr res(new MySQLResult(m_res), boost::bind(&LotosPP::Database::Driver::freeResult, this, boost::placeholders::_1));
	return verifyResult(res);
}

uint64_t MySQL::getLastInsertedRowID()
{
	return (uint64_t)mysql_insert_id(&m_handle);
}

std::string MySQL::escapeString(const std::string& s)
{
	return escapeBlob(s.c_str(), s.length());
}

std::string MySQL::escapeBlob(const char* s, uint32_t length)
{
	// remember about quoiting even an empty string!
	if (!s) {
		return "''";
		}

	// the worst case is 2n + 1
	char* output=new char[length*2+1];

	// quotes escaped string and frees temporary buffer
	mysql_real_escape_string(&m_handle, output, s, length);
	using namespace string_literals;
	string r{"'"s+output+"'"};
	delete[] output;
	return r;
}

void MySQL::freeResult(LotosPP::Database::Result* res)
{
	delete (MySQLResult*)res;
}

/** MySQLResult definitions */

MySQLResult::MySQLResult(MYSQL_RES* res)
{
	m_handle=res;
	m_listNames.clear();

	MYSQL_FIELD* field;
	int32_t i{0};
	while ((field=mysql_fetch_field(m_handle))!=nullptr) {
		m_listNames[field->name]=i;
		i++;
		}
}

MySQLResult::~MySQLResult()
{
	mysql_free_result(m_handle);
}

int32_t MySQLResult::getDataInt(const std::string& s)
{
	if (listNames_t::iterator it=m_listNames.find(s); it!=m_listNames.end()) {
		if (m_row[it->second]==NULL) {
			return 0;
			}
		return atoi(m_row[it->second]);
		}

	cout << "Error during getDataInt(" << s << ")." << endl;
	return 0; // Failed
}

uint32_t MySQLResult::getDataUInt(const std::string& s)
{
	if (listNames_t::iterator it=m_listNames.find(s); it!=m_listNames.end()) {
		if (m_row[it->second]==NULL) {
			return 0;
			}
		istringstream os(m_row[it->second]);
		uint32_t res;
		os >> res;
		return res;
		}

	cout << "Error during getDataInt(" << s << ")." << endl;
	return 0; // Failed
}

int64_t MySQLResult::getDataLong(const std::string& s)
{
	if (listNames_t::iterator it=m_listNames.find(s); it!=m_listNames.end()) {
		if (m_row[it->second]==NULL) {
			return 0;
			}
		return atoll(m_row[it->second]);
		}

	cout << "Error during getDataLong(" << s << ")." << endl;
	return 0; // Failed
}

std::string MySQLResult::getDataString(const std::string& s)
{
	if (listNames_t::iterator it=m_listNames.find(s); it!=m_listNames.end()) {
		return m_row[it->second]==NULL
			? ""
			: m_row[it->second];
		}

	cout << "Error during getDataString(" << s << ")." << endl;
	return ""; // Failed
}

const char* MySQLResult::getDataStream(const std::string& s, unsigned long& size)
{
	if (listNames_t::iterator it=m_listNames.find(s); it!=m_listNames.end()) {
		if (m_row[it->second]==NULL) {
			size=0;
			return NULL;
			}
		size=mysql_fetch_lengths(m_handle)[it->second];
		return m_row[it->second];
		}

	cout << "Error during getDataStream(" << s << ")." << endl;
	size=0;
	return NULL;
}

LotosPP::Database::Result_ptr MySQLResult::advance()
{
	m_row=mysql_fetch_row(m_handle);
	return m_row!=NULL
		? shared_from_this()
		: LotosPP::Database::Result_ptr();
}

bool MySQLResult::empty()
{
	return m_row==NULL;
}
