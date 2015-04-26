#ifndef LOTOS2_USER_H
#define	LOTOS2_USER_H


#include "config.h"

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include <boost/asio/ip/address.hpp>

#include "Creature.h"
#include "AutoList.h"
#include "network/Protocol.h"


namespace lotos2 {
	namespace network {
		namespace protocol {
			class Telnet;
			}
		}

class User
	: public Creature
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t userCount;
#endif
	User(const std::string& n="", network::Protocol* p=nullptr);
	virtual ~User();

	virtual User* getUser() { return this;};
	virtual const User* getUser() const { return this;};

	virtual const std::string& getName() const { return name;};

	virtual uint32_t idRange() { return 0x10000000;};
	static AutoList<User> listUser;
	void addList();
	void removeList();

	bool isOnline() const { return getID()!=0;};
	void disconnect() { if(client) client->disconnect();};
	boost::asio::ip::address getAddress() const;

	void uWrite(const std::string& message);

protected:
	std::string name;
	network::Protocol* client=nullptr;

	virtual void parseLine(const std::string& line);
	virtual void prompt();

	friend class Talker;
	friend class network::Protocol;
	friend class network::protocol::Telnet;
};

typedef std::vector<User*> UserVector;

} // namespace lotos2

#endif // LOTOS2_USER_H
