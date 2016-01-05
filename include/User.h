#ifndef LOTOSPP_USER_H
#define	LOTOSPP_USER_H


#include "config.h"

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include <boost/asio/ip/address.hpp>

#include "Creature.h"
#include "AutoList.h"
#include "network/Protocol.h"
#include "network/NetworkMessage.h"
#include "generated/enums.h"


namespace lotospp {
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
	virtual const std::vector<std::string>& getWords() const { return word;};

	virtual uint32_t idRange() { return 0x10000000;};
	static AutoList<User> listUser;
	void addList();
	void removeList();

	bool isOnline() const { return getID()!=0;};
	void disconnect() { if(client) client->disconnect();};
	boost::asio::ip::address getAddress() const;
	void kick();

	virtual void uRead(network::NetworkMessage msg);
	virtual void uWrite(const std::string& message);
	virtual void uPrintf(const std::string& fmtstr, ...);

protected:
	std::string name;
	network::Protocol* client=nullptr;
	std::string* password=nullptr;

	virtual void parseLine();
	virtual bool getCharclientLine(std::string& inpstr);
	virtual void prompt();
	void login(std::string inpstr);
	void attempt();
	void uConnect();

	UserLevel level=enums::UserLevel_LOGIN;
	UserStage stage=enums::UserStage_NEW;

	uint8_t attempts=0;
	int bpos=0;
	uint8_t buffnum=0;
	std::string buff;
	std::string textBuffer[2];
	std::string::iterator tbuff=textBuffer[buffnum].begin();
	int tbpos=0;
	std::string inlinePrompt;
	size_t buffpos=0;
	bool checho=false;
	std::vector<std::string> word;

	friend class Talker;
	friend class network::Protocol;
	friend class network::protocol::Telnet;

	int execCommand(std::string inpstr);
	void cmd_quit(std::string input);
	void cmd_say(std::string input);
};

typedef std::vector<User*> UserVector;

} // namespace lotospp

#endif // LOTOSPP_USER_H
