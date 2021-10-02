#ifndef LOTOSPP_COMMON_TALKER_H
#define	LOTOSPP_COMMON_TALKER_H

#include "AutoList.h"
#include <vector>


namespace LotosPP {
	namespace Network {
		class ServiceManager;
		}
	namespace Common {
		class Creature;
		class User;
		typedef std::vector<User*> UserVector;
		class Thing;

/**
 * Main Talker class
 * This class is responsible to control everything that happens
 */
class Talker
{
public:
	void start(LotosPP::Network::ServiceManager* servicer);

	/**
	 * Returns a creature based on the unique creature identifier
	 *
	 * @param id is the unique creature id to get a creature pointer to
	 * @return A Pointer to the creature
	 */
	Creature* getCreatureByID(uint32_t id);
	/**
	 * Returns a user based on the unique creature identifier
	 *
	 * @param id is the unique user id to get a user pointer to
	 * @return A Pointer to the user
	 */
	User* getUserByID(uint32_t id);

//	/**
//	 * Returns all users with a certain IP address
//	 *
//	 * @param ip is the IP address of the clients, as an unsigned long
//	 * @param mask An IP mask, default 255.255.255.255
//	 * @return A vector of all users with the selected IP
//	 */
//	UserVector getUsersByIP(uint32_t ip, uint32_t mask=0xFFFFFFFF);

	/**
	 * @param creature Creature to remove
	 */
	bool removeCreature(const Creature* creature);

	uint32_t getUsersOnline();

	void cleanup();
	void shutdown();

	void FreeThing(Thing* thing);

protected:
	std::vector<Thing*> toReleaseThings{};
	AutoList<Creature> listCreature{};
//	uint32_t maxUsers;
	LotosPP::Network::ServiceManager* service_manager{nullptr};
};

		}
	}

#endif
