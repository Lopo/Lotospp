#ifndef LOTOS2_DATABASE_TRANSACTION_H
#define	LOTOS2_DATABASE_TRANSACTION_H


#include "config.h"

#include "database/Driver.h"


namespace lotos2 {
	namespace database {

class Transaction
{
public:
	Transaction(Driver* database)
	{
		m_database=database;
		m_state=STATE_NO_START;
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
	TransactionStates_t m_state;
	Driver* m_database;
};

	} // namespace database
} // namespace lotos2

#endif // LOTOS2_DATABASE_TRANSACTION_H
