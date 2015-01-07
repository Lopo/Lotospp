#ifndef LOTOS2_NETWORK_PROTOCOL_H
#define	LOTOS2_NETWORK_PROTOCOL_H


#include "config.h"

#include <stdint.h>

#include <boost/noncopyable.hpp>
#include <boost/asio/ip/address.hpp>

#include "network/OutputMessage.h"
#include "network/Connection.h"


namespace lotos2 {
	namespace network {

class Protocol
	: boost::noncopyable
{
public:
	Protocol(Connection_ptr connection)
	{
		m_connection=connection;
		m_refCount=0;
	};

	virtual ~Protocol() {};

	virtual void parsePacket(NetworkMessage& msg)=0;

	void onSendMessage(OutputMessage_ptr msg);
	void onRecvMessage(NetworkMessage& msg);
	virtual void onRecvFirstMessage(NetworkMessage& msg)=0;
	virtual void onConnect() {}; // Used to send first packet to client

	Connection_ptr getConnection() { return m_connection;};
	const Connection_ptr getConnection() const { return m_connection;};
	void setConnection(Connection_ptr connection) { m_connection=connection;};

	boost::asio::ip::address getIP() const;
	int32_t addRef() { return ++m_refCount;};
	int32_t unRef() { return --m_refCount;};

protected:
	//Use this function for autosend messages only
	OutputMessage_ptr getOutputBuffer();

	virtual void releaseProtocol();
	virtual void deleteProtocolTask();
	friend class Connection;
private:
	OutputMessage_ptr m_outputBuffer;
	Connection_ptr m_connection;
	uint32_t m_refCount;
};

	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_PROTOCOL_H
