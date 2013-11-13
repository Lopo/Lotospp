#include <fstream>

#include "Lotos2/Talker.h"
#include "Lotos2/Scheduler.h"
#include "Lotos2/tasks.h"
#include "Lotos2/User.h"
#include "Lotos2/server.h"

#if defined __EXCEPTION_TRACER__
#include "Lotos2/Exception.h"
extern boost::recursive_mutex maploadlock;
#endif

void Talker::start(ServiceManager* servicer)
{
	service_manager=servicer;
}

User* Talker::getUserByID(uint32_t id)
{
	if (id==0) {
		return NULL;
		}

	AutoList<User>::listiterator it=User::listUser.list.find(id);
	if (it!=User::listUser.list.end()) {
		if (!it->second->isRemoved()) {
			return it->second;
			}
		}

	return NULL; //just in case the user doesnt exist
}

User* Talker::getUserByGuid(uint32_t guid)
{
	if (guid==0) {
		return NULL;
		}

	for (AutoList<User>::listiterator it=User::listUser.list.begin(); it!=User::listUser.list.end(); ++it) {
		if (!it->second->isRemoved()) {
			if (it->second->getGUID()==guid) {
				return it->second;
				}
			}
		}

	return NULL;
}

User* Talker::getUserByGuidEx(uint32_t guid)
{
	User* user=getUserByGuid(guid);
	if (user) {
		return user;
		}

	return user;
}

User* Talker::getUserByAccount(uint32_t acc)
{
	for (AutoList<User>::listiterator it=User::listUser.list.begin(); it!=User::listUser.list.end(); ++it) {
		if (!it->second->isRemoved()) {
			if (it->second->getAccountId()==acc) {
				return it->second;
				}
			}
		}

	return NULL;
}

UserVector Talker::getUsersByAccount(uint32_t acc)
{
	UserVector users;
	for (AutoList<User>::listiterator it=User::listUser.list.begin(); it != User::listUser.list.end(); ++it) {
		if (!it->second->isRemoved()) {
			if (it->second->getAccountId()==acc) {
				users.push_back(it->second);
				}
			}
		}

	return users;
}

UserVector Talker::getUsersByIP(uint32_t ipadress, uint32_t mask)
{
	UserVector users;
	for (AutoList<User>::listiterator it=User::listUser.list.begin(); it!=User::listUser.list.end(); ++it) {
		if (!it->second->isRemoved()) {
			if ((it->second->getIP() & mask)==(ipadress & mask)) {
				users.push_back(it->second);
				}
			}
		}

	return users;
}

bool Talker::removeUser(User* user)
{
	if (user->isRemoved()) {
		return false;
		}

	listUser.removeList(user->getID());
	user->onRemoved();

	return true;
}

uint32_t Talker::getUsersOnline()
{
	return (uint32_t)User::listUser.list.size();
}

void Talker::shutdown()
{
	std::cout << "Shutting down server...";

	g_scheduler.shutdown();
	g_dispatcher.shutdown();

	cleanup();

	if (service_manager) {
		service_manager->stop();
		}

	std::cout << "[done]" << std::endl;
}

void Talker::cleanup()
{
	//free memory
	for (std::vector<Thing*>::iterator it=toReleaseThings.begin(); it!=toReleaseThings.end(); ++it) {
		(*it)->unRef();
		}

	toReleaseThings.clear();
}

void Talker::FreeThing(Thing* thing)
{
	toReleaseThings.push_back(thing);
}
