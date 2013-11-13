#ifndef LOTOS2_TALKER_H
#define	LOTOS2_TALKER_H

#include "Lotos2/templates.h"
#include "Lotos2/User.h"
#include "Lotos2/server.h"

typedef std::vector<User*> UserVector;


/**
 * Main Talker class.
 * This class is responsible to control everything that happens
 */
class Talker
{
public:
	void start(ServiceManager* servicer);

	/**
	 * Returns a user based on the unique soul identifier
	 * \param id is the unique user id to get a user pointer to
	 * \return A Pointer to the user
	 */
	User* getUserByID(uint32_t id);

	/**
	 * Remove User from the map.
	 * Removes the User the map
	 * \param user User to remove
	 */
	bool removeUser(User* user);

	void cleanup();
	void shutdown();

	void FreeThing(Thing* thing);

protected:
	std::vector<Thing*> toReleaseThings;

	AutoList<User> listUser;

	uint32_t maxUsers;

	ServiceManager* service_manager;
};

extern Talker g_talker;

#endif	/* LOTOS2_TALKER_H */

