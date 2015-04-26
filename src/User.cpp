#include "config.h"

#include <cstdint>
#include <string>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/asio/ip/address.hpp>

#include "User.h"
#include "network/protocol/Telnet.h"
#include "network/OutputMessage.h"


using namespace lotos2;


AutoList<User> User::listUser;

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t User::userCount=0;
#endif


User::User(const std::string& n /*=""*/, network::Protocol* p/*=nullptr*/)
	: Creature(),
		name(n), client(p)
{
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	userCount++;
#endif

	if (client) {
		client->setUser(this);
		}
}

User::~User()
{
	if (client) {
		client->setUser(nullptr);
		}
	client=nullptr;
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	userCount--;
#endif
}

void User::addList()
{
	listUser.addList(this);
}

void User::removeList()
{
	listUser.removeList(getID());
}

boost::asio::ip::address User::getAddress() const
{
	if (client) {
		return client->getAddress();
		}
	return boost::asio::ip::address();
}

void User::parseLine(const std::string& line)
{
	uWrite(line+"\n");
	prompt();
}

void User::prompt()
{
	uWrite(">");
}

void User::uWrite(const std::string& message)
{
	if (client && message.length()) {
		client->write(message);
		}
}
