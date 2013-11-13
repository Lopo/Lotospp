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

bool Talker::removeUser(User* user)
{
	if (user->isRemoved()) {
		return false;
		}

	listUser.removeList(user->getID());
	user->onRemoved();

	return true;
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
