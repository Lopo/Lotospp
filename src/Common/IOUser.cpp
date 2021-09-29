#include "IOUser.h"
#include "Singleton.h"
#include "User.h"
#include "Database/Query.h"
#include "Database/Transaction.h"
#include "Database/Insert.h"
#include "Database/Result.h"


using namespace LotosPP::Common;


IOUser* IOUser::instance()
{
	static Singleton<IOUser> instance;
	return instance.get();
}

bool IOUser::load(User* user, const std::string& userName, bool preload/*=false*/)
{
	Database::Driver* db=Database::Driver::instance();
	Database::Query query;
	Database::Result_ptr result;

	query << "SELECT id, level, password \
		FROM `users` \
		WHERE login=LOWER(" << db->escapeString(userName) << ")";

	if (!(result=db->storeQuery(query))) {
		return false;
		}
	user->setGUID(result->getDataInt("id"));
	if (user->password) {
		user->password->assign(result->getDataString("password"));
		}
	else {
		user->password=new std::string(result->getDataString("password"));
		}

	if (preload) {
		//only loading basic info
		return true;
		}

	user->level=UserLevel::fromInteger(result->getDataUInt("level"));

	return true;
}

bool IOUser::save(const User* user, [[maybe_unused]]bool shallow/*=false*/)
{
	Database::Driver* db=Database::Driver::instance();
	Database::Query query;
	Database::Result_ptr result;

	query << "UPDATE users SET "
		<< " login=" << db->escapeString(user->name)
		<< ", level=" << user->level.value()
		<< " WHERE id=" << user->getGUID();
	Database::Transaction transaction(db);
	if (!transaction.begin()) {
		return false;
		}
	if (!db->executeQuery(query)) {
		return false;
		}
	return transaction.commit();
}

uint64_t IOUser::create(const User* user)
{
	Database::Driver* db=Database::Driver::instance();
	Database::Query query;
	Database::Transaction transaction(db);
	transaction.begin();
	Database::Insert insert(db);

	insert.setQuery("INSERT INTO users (login, password, level) VALUES ");
	query.reset();
	query << db->escapeString(user->name) << "," << db->escapeString(*user->password) << "," << user->level.value();
	if (!insert.addRow(query.str()) || !insert.execute()) {
		return false;
		}
	uint64_t id=db->getLastInsertedRowID();
	return transaction.commit()
		? id
		: 0;
}
