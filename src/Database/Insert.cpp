#include "Insert.h"
#include "Driver.h"


using namespace LotosPP::Database;


Insert::Insert(Driver* db)
	: m_db{db},
		m_multiLine{m_db->getParam(DBPARAM_MULTIINSERT)!=0} // checks if current database engine supports multi line INSERTs
{}

void Insert::setQuery(const std::string& query)
{
	m_query=query;
	m_buf.str("");
	m_rows=0;
}

bool Insert::addRow(const std::string& row)
{
	if (m_multiLine) {
		m_rows++;
		size_t size=m_buf.tellp();
		// adds new row to buffer
		if (!size) {
			m_buf << "(" << row << ")";
			}
		else if (size>8192) {
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
	return m_db->executeQuery(m_query+"("+row+")");
}

bool Insert::addRowAndReset(std::ostringstream& row)
{
	bool ret=addRow(row.str());
	row.str("");
	return ret;
}

bool Insert::execute()
{
	if (m_multiLine && m_buf.tellp()>0) {
		if (!m_rows) {
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
