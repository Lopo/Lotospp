#ifndef LOTOS2_NETWORK_PROTOCOL_H
#define	LOTOS2_NETWORK_PROTOCOL_H

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <stdint.h>

#include "Lotos2/network/OutputMessage.h"
#include "Lotos2/network/Connection.h"


class Protocol
	: boost::noncopyable
{
public:
	Protocol(Connection_ptr connection)
	{
		m_connection=connection;
		m_refCount=0;
	}

	virtual ~Protocol() {}

	virtual void parsePacket(NetworkMessage& msg, std::size_t bytes_transferred)=0;

	void onSendMessage(OutputMessage_ptr msg);
	void onRecvMessage(NetworkMessage& msg, std::size_t bytes_transferred);
	virtual void onRecvFirstMessage(NetworkMessage& msg, std::size_t bytes_transferred)=0;
	virtual void onConnect() {} // Used by new gameworld to send first packet to client

	Connection_ptr getConnection() { return m_connection;}
	const Connection_ptr getConnection() const { return m_connection;}
	void setConnection(Connection_ptr connection) { m_connection=connection; }

	int32_t addRef() { return ++m_refCount;}
	int32_t unRef() { return --m_refCount;}

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

#endif	/* LOTOS2_NETWORK_PROTOCOL_H */
