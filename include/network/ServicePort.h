#ifndef LOTOS2_NETWORK_SERVICEPORT_H
#define LOTOS2_NETWORK_SERVICEPORT_H


#include "config.h"

#include <stdint.h>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/noncopyable.hpp>

#include "network/Service.h"
#include "network/Protocol.h"
#include "network/Connection.h"


namespace lotos2 {
	namespace network {

typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> Acceptor_ptr;

/**
 * A Service Port represents a listener on a port
 *
 * It accepts connections, and asks each Service running on it if it can accept the connection, and if so passes
 * it on to the service
 */
class ServicePort
	: boost::noncopyable,
		public boost::enable_shared_from_this<ServicePort>
{
public:
	ServicePort(boost::asio::io_service& io_service);
	~ServicePort();

	static void openAcceptor(boost::weak_ptr<ServicePort> weak_service, uint16_t port);
	void open(uint16_t port);
	void close();
	bool is_single_socket() const;
	std::string get_protocol_names() const;

	bool add_service(Service_ptr);
	Protocol* make_protocol(NetworkMessage& msg) const;

	void onStopServer();
	void onAccept(Acceptor_ptr acceptor, boost::asio::ip::tcp::socket* socket, const boost::system::error_code& error);

protected:
	void accept(Acceptor_ptr acceptor);

	boost::asio::io_service& m_io_service;
	std::vector<Acceptor_ptr> m_tcp_acceptors;
	std::vector<Service_ptr> m_services;

	uint16_t m_serverPort;
	bool m_pendingStart;
	static bool m_logError;
};

typedef boost::shared_ptr<ServicePort> ServicePort_ptr;

	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_SERVICEPORT_H
