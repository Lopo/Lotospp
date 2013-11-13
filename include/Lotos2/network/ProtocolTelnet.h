#ifndef LOTOS2_NETWORK_PROTOCOLTELNET_H
#define LOTOS2_NETWORK_PROTOCOLTELNET_H

#include <list>

#include "Lotos2/network/Protocol.h"

class Talker;
class NetworkMessage;
extern Talker g_talker;


class User;
typedef boost::shared_ptr<NetworkMessage> NetworkMessage_ptr;

class ProtocolTelnet
	: public Protocol
{
public:
	// static protocol information
	enum { server_sends_first=true};
	enum { protocol_identifier=0}; // Not required as we send first
	static const char* protocol_name() { return "telnet protocol";}

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t protocolTelnetCount;
#endif

	ProtocolTelnet(Connection_ptr connection);
	virtual ~ProtocolTelnet();

	void setUser(User* p);

private:
	bool connect(uint32_t userId);
	void disconnectClient(const char* message);
	void disconnect();

	virtual void releaseProtocol();
	virtual void deleteProtocolTask();

	virtual void parsePacket(NetworkMessage& msg);
	virtual void onRecvFirstMessage(NetworkMessage& msg) { onRecvMessage(msg);};

	void sendTextMessage(const std::string& message);

	void AddTextMessage(NetworkMessage_ptr msg, const std::string& message);

	friend class User;

	// Helper so we don't need to bind every time
#define addTalkerTask(f, ...) addTalkerTaskInternal(false, 0, boost::bind(f, &g_talker, __VA_ARGS__))
#define addTalkerTaskTimed(delay, f, ...) addTalkerTaskInternal(true, delay, boost::bind(f, &g_talker, __VA_ARGS__))

	template<class FunctionType>
	void addTalkerTaskInternal(bool droppable, uint32_t delay, const FunctionType&);

	User* user;

	uint32_t eventConnect;
	bool m_debugAssertSent;
	bool m_acceptPackets;

	// Tell telnet to echo characters
	void sendEchoOn();
	// Tell telnet not to echo characters - for password entry etc.
	void sendEchoOff();

	template<typename _CharT>
	void setXtermTitle(const _CharT);
};

#endif	/* LOTOS2_NETWORK_PROTOCOLTELNET_H */
