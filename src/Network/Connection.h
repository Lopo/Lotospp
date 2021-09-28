#ifndef LOTOSPP_NETWORK_CONNECTION_H
#define LOTOSPP_NETWORK_CONNECTION_H

#include "NetworkMessage.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/system/error_code.hpp>
#include <boost/weak_ptr.hpp>
#ifdef __DEBUG_NET__
#	include <iostream>
#endif


namespace LotosPP::Network {
	class Connection;
	class ServicePort;
	typedef boost::shared_ptr<ServicePort> ServicePort_ptr;
	class Protocol;
	class OutputMessage;
	typedef boost::shared_ptr<OutputMessage> OutputMessage_ptr;

#ifdef __DEBUG_NET__
#	define PRINT_ASIO_ERROR(desc) \
	std::cout << "Error: [" << __FUNCTION__ << "] " << desc \
		<< " - Error: " << error.value() \
		<< " Desc: " << error.message() << std::endl;
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
	static inline uint32_t connectionCount{0};
#endif

	static const inline int64_t write_timeout{30};
	static const inline int64_t read_timeout{30};

	enum ConnectionState_t {
		CONNECTION_STATE_OPEN=0,
		CONNECTION_STATE_REQUEST_CLOSE,
		CONNECTION_STATE_CLOSING,
		CONNECTION_STATE_CLOSED
		};

	boost::asio::ip::tcp::socket& getHandle();

	void closeConnection();
	// Used by protocols that require server to send first
	void acceptConnection(Protocol* protocol);
	void acceptConnection();

	bool send(OutputMessage_ptr msg);

	boost::asio::ip::address getAddress() const;
	u_short getPort() const;
	std::string getHostname();

	int32_t addRef();
	int32_t unRef();

protected:
	boost::asio::io_service& getIoService()
	{
		return m_io_service;
	};

private:
	void parsePacket(const boost::system::error_code& error, const std::size_t bytes_transferred);

	void onWriteOperation(OutputMessage_ptr msg, const boost::system::error_code& error);

	void onStopOperation();
	void handleReadError(const boost::system::error_code& error);
	void handleWriteError(const boost::system::error_code& error);

	static void handleReadTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error);
	static void handleWriteTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error);
	static void handleResolveTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error);

	void closeConnectionTask();
	void deleteConnectionTask();
	void releaseConnection();
	void closeSocket();
	void onReadTimeout();
	void onWriteTimeout();
	void onResolveTimeout();

	void internalSend(OutputMessage_ptr msg);

	NetworkMessage m_msg;
	boost::asio::ip::tcp::socket* m_socket{nullptr};
	boost::asio::deadline_timer m_writeTimer;
	boost::asio::io_service& m_io_service;
	ServicePort_ptr m_service_port{nullptr};
	bool m_receivedFirst{false};
	bool m_writeError{false};
	bool m_readError{false};

	int32_t m_pendingWrite{0};
	int32_t m_pendingRead{0};
	ConnectionState_t m_connectionState{CONNECTION_STATE_OPEN};
	uint32_t m_refCount{0};
	static inline bool m_logError{true};
	boost::recursive_mutex m_connectionLock;

	Protocol* m_protocol{nullptr};

	std::string hostName;

	friend class Protocol;
};

typedef boost::shared_ptr<Connection> Connection_ptr;

	}

#endif
