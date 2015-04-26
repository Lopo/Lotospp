#ifndef LOTOS2_NETWORK_PROTOCOL_TELNET_H
#define LOTOS2_NETWORK_PROTOCOL_TELNET_H


#include "config.h"

#include <cstdint>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "network/Protocol.h"


namespace lotos2 {
	class User;

	namespace network {
		class NetworkMessage;
		typedef boost::shared_ptr<NetworkMessage> NetworkMessage_ptr;
		class Connection;
		typedef boost::shared_ptr<Connection> Connection_ptr;

		namespace protocol {

class Telnet
	: public Protocol
{
public:
	// static protocol information
	enum { server_sends_first=true};
	static const char* protocolName() { return "telnet protocol";};

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t protocolTelnetCount;
#endif

	Telnet(Connection_ptr connection);
	virtual ~Telnet();

private:
	bool connect(uint32_t userId);
	void disconnectClient(const char* message);

	virtual void releaseProtocol();
	virtual void deleteProtocolTask();

    virtual void onConnect();
	virtual void onRecvFirstMessage(NetworkMessage& msg);
	bool parseFirstPacket(NetworkMessage& msg);
	virtual void parsePacket(NetworkMessage& msg);

	void parseDebug(NetworkMessage& msg);

	friend class lotos2::User;

	// Helper so we don't need to bind every time
#define addTalkerTask(f, ...) addTalkerTaskInternal(false, 0, boost::bind(f, &g_talker, __VA_ARGS__))
#define addTalkerTaskTimed(delay, f, ...) addTalkerTaskInternal(true, delay, boost::bind(f, &g_talker, __VA_ARGS__))

	template<class FunctionType>
	void addTalkerTaskInternal(bool droppable, uint32_t delay, const FunctionType&);

	uint32_t eventConnect=0;
	bool m_debugAssertSent=false;
	bool m_acceptPackets=false;

	// Tell telnet to echo characters
	void sendEchoOn();
	// Tell telnet not to echo characters - for password entry etc.
	void sendEchoOff();

	void sendTermCoords();
	void setXtermTitle(const std::string& title);
	void enableLineWrap();
	void disableLineWrap();
};

		} // namespace protocol
	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_PROTOCOL_TELNET_H
