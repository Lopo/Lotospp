#include "Talker.h"

#include <iostream>

#include "globals.h"
#include "User.h"
#include "network/ServiceManager.h"
#if defined __EXCEPTION_TRACER__
#	include <boost/thread/recursive_mutex.hpp>
	extern boost::recursive_mutex maploadlock;
#endif


using namespace lotospp;


void Talker::start(network::ServiceManager* servicer)
{
	service_manager=servicer;
}

Creature* Talker::getCreatureByID(uint32_t id)
{
	if (id==0) {
		return nullptr;
		}

	AutoList<Creature>::listiterator it=listCreature.list.find(id);
	if (it!=listCreature.list.end()) {
//		if (!it->second->isRemoved()) {
//			return it->second;
//			}
		}

	return nullptr; //just in case the user doesnt exist
}

User* Talker::getUserByID(uint32_t id)
{
	if (id==0) {
		return nullptr;
		}

	AutoList<User>::listiterator it=User::listUser.list.find(id);
	if (it!=User::listUser.list.end()) {
//		if (!it->second->isRemoved()) {
//			return it->second;
//			}
		}

	return nullptr; //just in case the user doesnt exist
}

UserVector Talker::getUsersByIP(uint32_t ipadress, uint32_t mask)
{
	UserVector users;
	for (AutoList<User>::listiterator it=User::listUser.list.begin(); it!=User::listUser.list.end(); ++it) {
		}

	return users;
}

bool Talker::removeCreature(Creature* creature)
{
	listCreature.removeList(creature->getID());

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
