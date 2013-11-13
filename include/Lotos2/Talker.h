#ifndef LOTOS2_TALKER_H
#define	LOTOS2_TALKER_H

#include "Lotos2/templates.h"
#include "Lotos2/Account.h"
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
	 * Returns a user based on a guid identifier
	 * this function returns a pointer even if the user is offline,
	 * it is up to the caller of the function to delete the pointer - if the user is offline
	 * use isOffline() to determine if the user was offline
	 * \param guid is the identifier
	 * \return A Pointer to the user
	 */
	User* getUserByGuid(uint32_t guid);

	/**
	 * Returns a user based on a guid identifier
	 * this function returns a pointer even if the user is offline,
	 * it is up to the caller of the function to delete the pointer - if the user is offline
	 * use isOffline() to determine if the user was offline
	 * \param guid is the identifier
	 */
	User* getUserByGuidEx(uint32_t guid);

	/**
	 * Returns a user based on an account number identifier
	 * \param acc is the account identifier
	 * \return A Pointer to the user
	 */
	User* getUserByAccount(uint32_t acc);

	/**
	 * Returns all users based on their account number identifier
	 * \param acc is the account identifier
	 * \return A vector of all users with the selected account number
	 */
	UserVector getUsersByAccount(uint32_t acc);

	/**
	 * Returns all users with a certain IP address
	 * \param ip is the IP address of the clients, as an unsigned long
	 * \param mask An IP mask, default 255.255.255.255
	 * \return A vector of all users with the selected IP
	 */
	UserVector getUsersByIP(uint32_t ip, uint32_t mask=0xFFFFFFFF);

	/**
	 * Remove User from the map.
	 * Removes the User the map
	 * \param user User to remove
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

extern Talker g_talker;

#endif	/* LOTOS2_TALKER_H */

