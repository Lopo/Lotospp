#ifndef LOTOS2_TALKER_H
#define	LOTOS2_TALKER_H

#include "config.h"

#include "AutoList.h"
#include "structs/Account.h"
#include "User.h"
#include "server.h"


/**
 * Main Talker class
 * This class is responsible to control everything that happens
 */
class Talker
{
public:
	void start(ServiceManager* servicer);

	/**
	 * Returns a user based on the unique soul identifier
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
	 * Remove User from the map
	 *
	 * Removes the User the map
	 *
	 * @param user User to remove
	 */
	bool removeUser(User* user);

	uint32_t getUsersOnline();

	void cleanup();
	void shutdown();

	void FreeThing(Thing* thing);

protected:
	std::vector<Thing*> toReleaseThings;

	AutoList<User> listUser;

	uint32_t maxUsers;

	ServiceManager* service_manager;
};

#endif /* LOTOS2_TALKER_H */
