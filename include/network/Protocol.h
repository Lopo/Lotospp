#ifndef LOTOS2_NETWORK_PROTOCOL_H
#define	LOTOS2_NETWORK_PROTOCOL_H


#include "config.h"

#include <cstdint>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/address.hpp>


namespace lotos2 {
	class User;

	namespace network {
		class Connection;
		typedef boost::shared_ptr<Connection> Connection_ptr;
		class NetworkMessage;
		class OutputMessage;
		typedef boost::shared_ptr<OutputMessage> OutputMessage_ptr;

class Protocol
	: boost::noncopyable
{
public:
	Protocol(Connection_ptr connection)
	{
		m_connection=connection;
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

	boost::asio::ip::address getAddress() const;
	int32_t addRef() { return ++m_refCount;};
	int32_t unRef() { return --m_refCount;};

	void setUser(lotos2::User* p);

	virtual void write(const std::string& str);

protected:
	//Use this function for autosend messages only
	OutputMessage_ptr getOutputBuffer();

	virtual void releaseProtocol();
	virtual void deleteProtocolTask();
	virtual void disconnect();

	lotos2::User* user=nullptr;

	friend class Connection;
	friend class lotos2::User;
private:
	OutputMessage_ptr m_outputBuffer=nullptr;
	Connection_ptr m_connection=nullptr;
	uint32_t m_refCount=0;
};

	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_PROTOCOL_H
