#ifndef LOTOSPP_DATABASE_INSERT_H
#define	LOTOSPP_DATABASE_INSERT_H

#include <string>
#include <sstream>
#include <cstdint>


namespace LotosPP::Database {
	class Driver;

/**
 * INSERT statement
 *
 * Gives possibility to optimize multiple INSERTs on databases that support multiline INSERTs
 */
class Insert
{
public:
	/**
	 * Associates with given database handler
	 *
	 * @param db database wrapper
	 */
	Insert(Driver* db);
	~Insert()
	{};

	/**
	 * Sets query prototype
	 *
	 * @param query INSERT query
	 */
	void setQuery(const std::string& query);

	/**
	 * Adds new row to INSERT statement
	 *
	 * On databases that doesn't support multiline INSERTs it simply execute INSERT for each row
	 *
	 * @param row row data
	 */
	bool addRow(const std::string& row);
	/**
	 * Allows to use addRow() with stringstream as parameter
	 * This also clears the stringstream!
	 */
	bool addRowAndReset(std::ostringstream& row);

	/**
	 * Executes current buffer
	 */
	bool execute();

	/**
	 * Returns ID of the inserted column if it had a AUTO_INCREMENT key
	 */
	uint64_t getInsertID();

protected:
	Driver* m_db{nullptr};
	bool m_multiLine{false};
	uint32_t m_rows{0};
	std::string m_query;
	std::ostringstream m_buf;
};

	}

#endif
