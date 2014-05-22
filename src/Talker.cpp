#include <fstream>

#include "Talker.h"
#include "Scheduler.h"
#include "tasks.h"
#include "User.h"
#include "server.h"

#include "globals.h"

#if defined __EXCEPTION_TRACER__
#	include "Exception.h"
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
		}

	return NULL; //just in case the user doesnt exist
}

UserVector Talker::getUsersByIP(uint32_t ipadress, uint32_t mask)
{
	UserVector users;
	for (AutoList<User>::listiterator it=User::listUser.list.begin(); it!=User::listUser.list.end(); ++it) {
		}

	return users;
}

bool Talker::removeUser(User* user)
{
	listUser.removeList(user->getID());

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
