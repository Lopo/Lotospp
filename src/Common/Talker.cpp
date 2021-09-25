#include "Talker.h"
#include "globals.h"
#include "User.h"
#include "Network/ServiceManager.h"
#ifdef __EXCEPTION_TRACER__
#	include <boost/thread/recursive_mutex.hpp>
	extern boost::recursive_mutex maploadlock;
#endif
#include <iostream>


using namespace LotosPP::Common;


void Talker::start(Network::ServiceManager* servicer)
{
	service_manager=servicer;
}

Creature* Talker::getCreatureByID(uint32_t id)
{
	if (!id) {
		return nullptr;
		}

	if (AutoList<Creature>::listiterator it=listCreature.list.find(id); it!=listCreature.list.end()) {
//		if (!it->second->isRemoved()) {
//			return it->second;
//			}
		}

	return nullptr; //just in case the user doesnt exist
}

User* Talker::getUserByID(uint32_t id)
{
	if (!id) {
		return nullptr;
		}

	if (AutoList<User>::listiterator it=User::listUser.list.find(id); it!=User::listUser.list.end()) {
//		if (!it->second->isRemoved()) {
//			return it->second;
//			}
		}

	return nullptr; //just in case the user doesnt exist
}

UserVector Talker::getUsersByIP(uint32_t ipadress, uint32_t mask)
{
	UserVector users{};
	for (auto& it : User::listUser.list) {
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

	LotosPP::g_scheduler.shutdown();
	LotosPP::g_dispatcher.shutdown();

	cleanup();

	if (service_manager) {
		service_manager->stop();
		}

	std::cout << "[done]" << std::endl;
}

void Talker::cleanup()
{
	//free memory
	for (auto& it : toReleaseThings) {
		it->unRef();
		}

	toReleaseThings.clear();
}

void Talker::FreeThing(Thing* thing)
{
	toReleaseThings.push_back(thing);
}
