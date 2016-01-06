#ifndef LOTOSPP_IOUSER_H
#define LOTOSPP_IOUSER_H


#include "config.h"

#include <string>


namespace lotospp {
	class User;

class IOUser {
public:
	static IOUser* instance();

	/**
	 * Load a user
	 *
	 * @param user User object to load to
	 * @param userName Name of the user
	 * @param preLoad if set to true only group, guid and account id will be loaded, default: false
	 * @return true if the user was successfully loaded
	 */
	bool load(User* user, const std::string& userName, bool preLoad=false);
	/**
	 * Save a user
	 * @param user the user to save
	 * @param shallow
	 * @return true if the user was successfully saved
	 */
	bool save(const User* user, bool shallow=false);
	uint64_t create(const User* user);
	bool getPassword(const std::string& userName, std::string& password);
};

} //namespace lotospp

#endif // LOTOSPP_IOUSER_H
