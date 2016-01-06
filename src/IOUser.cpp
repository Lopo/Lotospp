#include "IOUser.h"
#include "Singleton.h"
#include "database/Driver.h"
#include "database/Query.h"
#include "database/Transaction.h"
#include "database/Insert.h"
#include "database/Result.h"
#include "User.h"


using namespace lotospp;


IOUser* IOUser::instance()
{
	static Singleton<IOUser> instance;
	return instance.get();
}

bool IOUser::load(User* user, const std::string& userName, bool preload/*=false*/)
{
	database::Driver* db=database::Driver::instance();
	database::Query query;
	database::Result_ptr result;

	query << "SELECT id, level, password \
		FROM `users` \
		WHERE login=" << db->escapeString(userName);

	if (!(result=db->storeQuery(query))) {
		return false;
		}
	user->setGUID(result->getDataInt("id"));
	if (user->password) {
		delete user->password;
		}
	user->password=new std::string(result->getDataString("password"));

	if (preload) {
		//only loading basic info
		return true;
		}

	user->level=UserLevel::fromInteger(result->getDataUInt("level"));

	return true;
}

bool IOUser::save(const User* user, bool shallow/*=false*/)
{
	database::Driver* db=database::Driver::instance();
	database::Query query;
	database::Result_ptr result;

	query << "UPDATE users SET "
		<< " login=" << db->escapeString(user->name)
		<< ", level=" << user->level.value()
		;
	query << " WHERE id=" << user->getGUID();
	database::Transaction transaction(db);
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
	database::Driver* db=database::Driver::instance();
	database::Query query;
	database::Transaction transaction(db);
	transaction.begin();
	database::Insert insert(db);

	insert.setQuery("INSERT INTO users (login, password, level) VALUES");
	query.reset();
	query << db->escapeString(user->name) << "," << db->escapeString(*user->password) << "," << user->level.value();
	if (!insert.addRow(query.str())) {
		return false;
		}
	if (!insert.execute()) {
		return false;
		}
	uint64_t id=db->getLastInsertedRowID();
	return transaction.commit()
		? id
		: 0;
}
