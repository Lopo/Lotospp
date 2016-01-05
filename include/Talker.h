#ifndef LOTOSPP_TALKER_H
#define	LOTOSPP_TALKER_H


#include "config.h"

#include <cstdint>
#include <vector>

#include "AutoList.h"


namespace lotospp {
	class Creature;
	class User;
	typedef std::vector<User*> UserVector;
	class Thing;
	namespace network {
		class ServiceManager;
		}

/**
 * Main Talker class
 * This class is responsible to control everything that happens
 */
class Talker
{
public:
	void start(network::ServiceManager* servicer);

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

	/**
	 * Returns all users with a certain IP address
	 *
	 * @param ip is the IP address of the clients, as an unsigned long
	 * @param mask An IP mask, default 255.255.255.255
	 * @return A vector of all users with the selected IP
	 */
	UserVector getUsersByIP(uint32_t ip, uint32_t mask=0xFFFFFFFF);

	/**
	 * @param creature Creature to remove
	 */
	bool removeCreature(Creature* creature);

	uint32_t getUsersOnline();

	void cleanup();
	void shutdown();

	void FreeThing(Thing* thing);

protected:
	std::vector<Thing*> toReleaseThings;

	AutoList<Creature> listCreature;

	uint32_t maxUsers;

	network::ServiceManager* service_manager;
};

} // namespace lotospp

#endif // LOTOSPP_TALKER_H
