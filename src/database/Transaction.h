#ifndef LOTOSPP_DATABASE_TRANSACTION_H
#define	LOTOSPP_DATABASE_TRANSACTION_H


#include "config.h"
#include "database/Driver.h"


namespace lotospp {
	namespace database {

class Transaction
{
public:
	Transaction(Driver* database)
	{
		m_database=database;
	};

	~Transaction()
	{
		if (m_state==STATE_START) {
			m_database->rollback();
			}
	};

	bool begin()
	{
		m_state=STATE_START;
		return m_database->beginTransaction();
	};

	bool commit()
	{
		if (m_state==STATE_START) {
			m_state=STEATE_COMMIT;
			return m_database->commit();
			}
		return false;
	};

private:
	enum TransactionStates_t {
		STATE_NO_START,
		STATE_START,
		STEATE_COMMIT
		};
	TransactionStates_t m_state=STATE_NO_START;
	Driver* m_database;
};

	} // namespace database
} // namespace lotospp

#endif // LOTOSPP_DATABASE_TRANSACTION_H
