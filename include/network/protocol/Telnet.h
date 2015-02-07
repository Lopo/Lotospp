#ifndef LOTOS2_NETWORK_PROTOCOL_TELNET_H
#define LOTOS2_NETWORK_PROTOCOL_TELNET_H


#include "config.h"

#include <stdint.h>

#include <string>

#include "network/Protocol.h"
#include "network/NetworkMessage.h"


namespace lotos2 {
	class User;

	namespace network {

typedef boost::shared_ptr<NetworkMessage> NetworkMessage_ptr;

		namespace protocol {

class Telnet
	: public Protocol
{
public:
	// static protocol information
	enum { server_sends_first=true};
	enum { protocol_identifier=0}; // Not required as we send first
	static const char* protocol_name() { return "telnet protocol";};

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t protocolTelnetCount;
#endif

	Telnet(Connection_ptr connection);
	virtual ~Telnet();

	void setUser(lotos2::User* p);

private:
	bool connect(uint32_t userId);
	void disconnectClient(const char* message);
	void disconnect();

	virtual void releaseProtocol();
	virtual void deleteProtocolTask();

    virtual void onConnect();
	virtual void onRecvFirstMessage(NetworkMessage& msg);
	bool parseFirstPacket(NetworkMessage& msg);
	virtual void parsePacket(NetworkMessage& msg);

	void parseDebug(NetworkMessage& msg);

	void sendTextMessage(const std::string& message);

	void AddTextMessage(NetworkMessage_ptr msg, const std::string& message);

	friend class lotos2::User;

	// Helper so we don't need to bind every time
#define addTalkerTask(f, ...) addTalkerTaskInternal(false, 0, boost::bind(f, &g_talker, __VA_ARGS__))
#define addTalkerTaskTimed(delay, f, ...) addTalkerTaskInternal(true, delay, boost::bind(f, &g_talker, __VA_ARGS__))

	template<class FunctionType>
	void addTalkerTaskInternal(bool droppable, uint32_t delay, const FunctionType&);

	lotos2::User* user=nullptr;

	uint32_t eventConnect=0;
	bool m_debugAssertSent=false;
	bool m_acceptPackets=false;

	// Tell telnet to echo characters
	void sendEchoOn();
	// Tell telnet not to echo characters - for password entry etc.
	void sendEchoOff();

	void sendTermCoords();
	template<typename _CharT>
	void setXtermTitle(const _CharT);
	void enableLineWrap();
	void disableLineWrap();
};

		} // namespace protocol
	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_PROTOCOL_TELNET_H
