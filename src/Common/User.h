#ifndef LOTOSPP_COMMON_USER_H
#define	LOTOSPP_COMMON_USER_H

#include "config.h"
#include "Creature.h"
#include "AutoList.h"
#include "Network/Protocol.h"
#include "Network/NetworkMessage.h"
#include "Common/Enums/UserStage.h"
#include "Common/Enums/TelnetFlag.h"
#include "Common/Enums/UserLevel.h"
#include "Strings/Splitline.h"
#include <ostream>
#include <string>
#include <vector>
#include <cstdint>


namespace LotosPP {
	namespace Network::Protocols {
		class Telnet;
		}
	namespace Common {

class User
	: public Creature
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static inline uint32_t userCount{0};
#endif
	User(const std::string& n="", LotosPP::Network::Protocol* p=nullptr);
	virtual ~User();

	virtual User* getUser()
	{
		return this;
	};
	virtual const User* getUser() const
	{
		return this;
	};

	virtual const std::string& getName() const
	{
		return name;
	};

	void setGUID(const uint32_t _guid)
	{
		guid=_guid;
	}
	uint32_t getGUID() const
	{
		return guid;
	}

	virtual uint32_t idRange()
	{
		return 0x10000000;
	};
	static inline AutoList<User> listUser{};
	void addList();
	void removeList();

	bool isOnline() const
	{
		return getID()!=0;
	};
	void disconnect()
	{
		if (client) {
			client->disconnect();
			}
	};
	boost::asio::ip::address getAddress() const;
	void kick();

	virtual void uRead(LotosPP::Network::NetworkMessage msg);
	virtual void uWrite(const std::string& message);
	virtual void uPrintf(const char* fmtstr, ...);

	LotosPP::Strings::Splitline com;
	LotosPP::Network::Protocol* client{nullptr};
	UserStage stage{enums::UserStage_NEW};

protected:
	std::string name;
	std::string* password{nullptr};

	virtual void parseLine();
	virtual void prompt();
	void login(std::string inpstr);
	void attempt();
	void uConnect();
	bool parseTelopt();
	int getTermsize();
	int getTermtype();
	TelnetFlag flagsTelnet;

	UserLevel level{enums::UserLevel_LOGIN};

	uint8_t attempts{0};
	uint32_t bpos{0};
	uint8_t buffnum{0};
	std::string buff;
	std::string textBuffer[2];
	int tbpos{0};
	std::string inlinePrompt;
	size_t buffpos{0};
	bool checho{false};
	uint16_t termCols{80}, termRows{25};
	std::string termType;

	uint32_t guid{0};

	friend class Talker;
	friend class LotosPP::Network::Protocol;
	friend class LotosPP::Network::Protocols::Telnet;
	friend class IOUser;

	virtual void runCmdLine();
};

typedef std::vector<User*> UserVector;

		}
	}

#endif
