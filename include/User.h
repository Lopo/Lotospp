#ifndef LOTOS2_USER_H
#define	LOTOS2_USER_H


#include "config.h"

#include <stdint.h>

#include <ostream>
#include <string>
#include <vector>

#include <boost/asio/ip/address.hpp>

#include "Creature.h"
#include "AutoList.h"
#include "network/protocol/Telnet.h"


namespace lotos2 {

class User
	: public Creature,
		public std::ostream
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t userCount;
#endif
	User(const std::string& name, network::protocol::Telnet* p);
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
	boost::asio::ip::address getIP() const;

	void sendTextMessage(const std::string& message) const { if (client) client->sendTextMessage(message);};

	template<typename _CharT>
	void uWrite(const _CharT message);

protected:
	network::protocol::Telnet* client;

	std::string name;

	virtual void parseLine(const std::string& line);
	virtual void prompt();

	friend class Talker;
	friend class network::protocol::Telnet;
};

typedef std::vector<User*> UserVector;

} // namespace lotos2

#endif // LOTOS2_USER_H
