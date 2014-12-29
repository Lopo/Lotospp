#ifndef LOTOS2_USER_H
#define	LOTOS2_USER_H

#include "config.h"

#include <string>
#include <vector>
#include <ostream>

#include <boost/asio/ip/address.hpp>

#include "Creature.h"
#include "network/ProtocolTelnet.h"
#include "AutoList.h"


class User
	: public Creature,
		public std::ostream
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t userCount;
#endif
	User(const std::string& name, ProtocolTelnet* p);
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
	ProtocolTelnet* client;

	std::string name;

	virtual void parseLine(const std::string& line);
	virtual void prompt();

	friend class Talker;
	friend class ProtocolTelnet;
};

typedef std::vector<User*> UserVector;

#endif /* LOTOS2_USER_H */
