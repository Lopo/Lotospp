#ifndef LOTOS2_NETWORK_CONNECTION_H
#define LOTOS2_NETWORK_CONNECTION_H


#include "config.h"

#ifdef __DEBUG_NET__
#	include <iostream>
#endif

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/system/error_code.hpp>
#include <boost/weak_ptr.hpp>

#include "network/NetworkMessage.h"


namespace lotos2 {
	namespace network {
		class ServicePort;
		typedef boost::shared_ptr<ServicePort> ServicePort_ptr;
		class Protocol;
		class OutputMessage;
		typedef boost::shared_ptr<OutputMessage> OutputMessage_ptr;

#ifdef __DEBUG_NET__
#	define PRINT_ASIO_ERROR(desc) \
	std::cout << "Error: [" << __FUNCTION__ << "] " << desc << " - Error: " << \
		error.value() << " Desc: " << error.message() << std::endl;
#else
#	define PRINT_ASIO_ERROR(desc)
#endif

class Connection
	: public boost::enable_shared_from_this<Connection>,
		boost::noncopyable
{
	friend class ConnectionManager;

public:
	Connection(boost::asio::ip::tcp::socket* socket, boost::asio::io_service& io_service, ServicePort_ptr service_port);
	~Connection();

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t connectionCount;
#endif

	enum { write_timeout=30};
	enum { read_timeout=30};

	enum ConnectionState_t {
		CONNECTION_STATE_OPEN=0,
		CONNECTION_STATE_REQUEST_CLOSE=1,
		CONNECTION_STATE_CLOSING=2,
		CONNECTION_STATE_CLOSED=3
		};

	boost::asio::ip::tcp::socket& getHandle();

	void closeConnection();
	// Used by protocols that require server to send first
	void acceptConnection(Protocol* protocol);
	void acceptConnection();

	bool send(OutputMessage_ptr msg);

	boost::asio::ip::address getIP() const;

	int32_t addRef();
	int32_t unRef();

private:
	void parsePacket(const boost::system::error_code& error, const std::size_t bytes_transferred);

	void onWriteOperation(OutputMessage_ptr msg, const boost::system::error_code& error);

	void onStopOperation();
	void handleReadError(const boost::system::error_code& error);
	void handleWriteError(const boost::system::error_code& error);

	static void handleReadTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error);
	static void handleWriteTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error);

	void closeConnectionTask();
	void deleteConnectionTask();
	void releaseConnection();
	void closeSocket();
	void onReadTimeout();
	void onWriteTimeout();

	void internalSend(OutputMessage_ptr msg);

	NetworkMessage m_msg;
	boost::asio::ip::tcp::socket* m_socket;
	boost::asio::deadline_timer m_writeTimer;
	boost::asio::io_service& m_io_service;
	ServicePort_ptr m_service_port;
	bool m_receivedFirst=false;
	bool m_writeError=false;
	bool m_readError=false;

	int32_t m_pendingWrite=0;
	int32_t m_pendingRead=0;
	ConnectionState_t m_connectionState=CONNECTION_STATE_OPEN;
	uint32_t m_refCount=0;
	static bool m_logError;
	boost::recursive_mutex m_connectionLock;

	Protocol* m_protocol=nullptr;
};

typedef boost::shared_ptr<Connection> Connection_ptr;

	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_CONNECTION_H
