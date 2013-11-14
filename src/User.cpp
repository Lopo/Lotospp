#include "User.h"
#include "Scheduler.h"
#include "Talker.h"

#if defined __EXCEPTION_TRACER__
#include "Exception.h"
#endif

boost::recursive_mutex AutoID::autoIDLock;
uint32_t AutoID::count=1000;
AutoID::list_type AutoID::list;

AutoList<User> User::listUser;

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t User::userCount=0;
#endif

User::User(ProtocolTelnet* p)
	: isInternalRemoved(false)
{
	id=0;
	client=p;
	isConnecting=false;
	if (client) {
		client->setUser(this);
		}

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	userCount++;
#endif
}

User::~User()
{
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	userCount--;
#endif
}

bool User::isRemoved() const
{
	return isInternalRemoved;
}

void User::onRemoved()
{
	removeList();
	setRemoved();
}

void User::setID()
{
	this->id=auto_id | this->idRange();
}

void User::setRemoved()
{
	isInternalRemoved=true;
}

uint32_t User::getID() const
{
	return id;
}

void User::removeList()
{
	listUser.removeList(getID());
}

void User::addList()
{
	listUser.addList(this);
}
