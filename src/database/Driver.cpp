#include "database/Driver.h"
#include "database/MySQL.h"

boost::recursive_mutex DBQuery::database_lock;

DatabaseDriver* DatabaseDriver::_instance=NULL;

DatabaseDriver* DatabaseDriver::instance()
{
	if (!_instance) {
		_instance=new DatabaseMySQL;
		}
	return _instance;
}

bool DatabaseDriver::executeQuery(DBQuery &query)
{
	return internalQuery(query.str());
}

bool DatabaseDriver::executeQuery(const std::string &query)
{
	return internalQuery(query);
}

DBResult_ptr DatabaseDriver::storeQuery(const std::string &query)
{
	return internalSelectQuery(query);
}

DBResult_ptr DatabaseDriver::storeQuery(DBQuery &query)
{
	return storeQuery(query.str());
}

void DatabaseDriver::freeResult(DBResult *res)
{
	throw std::runtime_error("No database driver loaded, yet a DBResult was freed.");
}

DBResult_ptr DatabaseDriver::verifyResult(DBResult_ptr result)
{
	return result->advance()
		? result
		: DBResult_ptr();
}

// DBQuery

DBQuery::DBQuery()
{
	database_lock.lock();
}

DBQuery::~DBQuery()
{
	database_lock.unlock();
}

// DBInsert

DBInsert::DBInsert(DatabaseDriver* db)
{
	m_db=db;
	m_rows=0;

	// checks if current database engine supports multi line INSERTs
	m_multiLine= m_db->getParam(DBPARAM_MULTIINSERT)!=0;
}

void DBInsert::setQuery(const std::string& query)
{
	m_query=query;
	m_buf.str("");
	m_rows=0;
}

bool DBInsert::addRow(const std::string& row)
{
	if (m_multiLine) {
		m_rows++;
		size_t size=m_buf.tellp();

		// adds new row to buffer
		if (size==0) {
			m_buf << "(" << row << ")";
			}
		else if(size > 8192) {
			if (!execute()) {
				return false;
				}
			m_buf << "(" << row << ")";
			}
		else {
			m_buf << ",(" + row + ")";
			}

		return true;
		}
	// executes INSERT for current row
	return m_db->executeQuery(m_query + "(" + row + ")" );
}

bool DBInsert::addRowAndReset(std::ostringstream& row)
{
	bool ret=addRow(row.str());
	row.str("");
	return ret;
}

bool DBInsert::execute()
{
	if (m_multiLine && m_buf.tellp()>0) {
		if (m_rows==0) {
			//no rows to execute
			return true;
			}
		// executes buffer
		bool res=m_db->executeQuery(m_query+m_buf.str());

		// Reset counters
		m_rows=0;
		m_buf.str("");
		return res;
		}
	// INSERTs were executed on-fly
	return true;
}
