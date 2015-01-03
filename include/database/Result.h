#ifndef LOTOS2_DATABASE_RESULT_H
#define	LOTOS2_DATABASE_RESULT_H

#include "config.h"

#include <boost/enable_shared_from_this.hpp>


namespace lotos2 {
	namespace database {

class Result;
typedef boost::shared_ptr<Result> Result_ptr;

class Result
	: public boost::enable_shared_from_this<Result>
{
public:
	/**
	 * Get the Integer value of a field in database
	 *
	 * @param s The name of the field
	 * @return The Integer value of the selected field and row
	 */
	virtual int32_t getDataInt(const std::string &s) { return 0;};
	/**
	 * Get the Unsigned Integer value of a field in database
	 *
	 * @param s The name of the field
	 * @return The Integer value of the selected field and row
	 */
	virtual uint32_t getDataUInt(const std::string &s) { return 0;};
	/**
	 * Get the Long value of a field in database
	 *
	 * @param s The name of the field
	 * @return The Long value of the selected field and row
	 */
	virtual int64_t getDataLong(const std::string &s) { return 0;};
	/**
	 * Get the String of a field in database
	 *
	 * @param s The name of the field
	 * @return The String of the selected field and row
	 */
	virtual std::string getDataString(const std::string &s) { return "''";};
	/**
	 * Get the blob of a field in database
	 *
	 * @param s The name of the field
	 * @return a PropStream that is initiated with the blob data field, if not exist it returns NULL
	 */
	virtual const char* getDataStream(const std::string &s, unsigned long &size) { return 0;};

	/**
	 * Moves to next result in set
	 *
	 * @return true if moved, false if there are no more results
	 */
	virtual Result_ptr advance() { return Result_ptr();};

	/**
	 * Are there any more rows to be fetched
	 *
	 * @return true if there are no more rows
	 */
	virtual bool empty() { return true;};

protected:
	Result() {};
	virtual ~Result() {};
};

	} // namespace database
} // namespace lotos2

#endif /* LOTOS2_DATABASE_RESULT_H */
