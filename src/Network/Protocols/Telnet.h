#ifndef LOTOSPP_NETWORK_PROTOCOLS_TELNET_H
#define LOTOSPP_NETWORK_PROTOCOLS_TELNET_H

#include "../Protocol.h"
#include <boost/bind/bind.hpp>


namespace LotosPP {
	namespace Common {
		class User;
		}
	namespace Network {
		class NetworkMessage;
		class Connection;
		typedef boost::shared_ptr<Connection> Connection_ptr;

		namespace Protocols {

class Telnet
	: public LotosPP::Network::Protocol
{
public:
	// static protocol information
	enum {
		server_sends_first=true
		};

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static inline uint32_t protocolTelnetCount{0};
#endif

	Telnet(Connection_ptr connection);
	virtual ~Telnet();
	static const char* protocolName()
	{
		return "Telnet protocol";
	};

protected:
	// Tell telnet to echo characters
	virtual void sendEchoOn();
	// Tell telnet not to echo characters - for password entry etc.
	virtual void sendEchoOff();

	void sendTermCoords();
	void setXtermTitle(const std::string& title);
	void enableLineWrap();
	void disableLineWrap();

private:
	bool connect(uint32_t userId);
	void disconnectClient(const char* message);

	virtual void releaseProtocol();
	virtual void deleteProtocolTask();

    virtual void onConnect();
	virtual void onRecvFirstMessage(NetworkMessage& msg);
	bool parseFirstPacket(NetworkMessage& msg);
	virtual void parsePacket(NetworkMessage& msg);

	friend class LotosPP::Common::User;

	// Helper so we don't need to bind every time
#define addTalkerTask(f, ...) addTalkerTaskInternal(false, 0, boost::bind(f, &g_talker, __VA_ARGS__))
#define addTalkerTaskTimed(delay, f, ...) addTalkerTaskInternal(true, delay, boost::bind(f, &g_talker, __VA_ARGS__))

	template<typename FunctionType>
	void addTalkerTaskInternal(bool droppable, uint32_t delay, const FunctionType&);

	uint32_t eventConnect{0};
	bool m_debugAssertSent{false};
	bool m_acceptPackets{false};
};

			}
		}
	}

#endif
