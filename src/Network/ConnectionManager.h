#ifndef LOTOSPP_NETWORK_CONNECTIONMANAGER_H
#define LOTOSPP_NETWORK_CONNECTIONMANAGER_H

#include "config.h"
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <list>


namespace LotosPP::Network {
	class Connection;
	typedef boost::shared_ptr<Connection> Connection_ptr;
	class ServicePort;
	typedef boost::shared_ptr<ServicePort> ServicePort_ptr;

class ConnectionManager
{
public:
	static ConnectionManager* getInstance();

	Connection_ptr createConnection(boost::asio::ip::tcp::socket* socket, boost::asio::io_service& io_service, ServicePort_ptr servicers);
	void releaseConnection(Connection_ptr connection);
	void closeAll();

protected:
	std::list<Connection_ptr> m_connections{};
	boost::recursive_mutex m_connectionManagerLock;
};

	}

#endif
