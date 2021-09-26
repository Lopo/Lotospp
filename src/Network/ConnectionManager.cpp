#include "ConnectionManager.h"
#include "Connection.h"
#include "Common/Singleton.h"
#include <boost/shared_ptr.hpp>
#include <boost/system/system_error.hpp>


using namespace LotosPP::Network;


ConnectionManager* ConnectionManager::getInstance()
{
	static LotosPP::Common::Singleton<ConnectionManager> instance;
	return instance.get();
}

Connection_ptr ConnectionManager::createConnection(boost::asio::ip::tcp::socket* socket, boost::asio::io_service& io_service, ServicePort_ptr servicer)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Create new Connection" << std::endl;
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionManagerLock);
	Connection_ptr connection=boost::shared_ptr<Connection>(new Connection(socket, io_service, servicer));
	m_connections.push_back(connection);
	return connection;
}

void ConnectionManager::releaseConnection(Connection_ptr connection)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Releasing connection" << std::endl;
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionManagerLock);
	if (std::list<Connection_ptr>::iterator it=std::find(m_connections.begin(), m_connections.end(), connection); it!=m_connections.end()) {
		m_connections.erase(it);
		}
	else {
		std::cout << "Error: [ConnectionManager::releaseConnection] Connection not found" << std::endl;
		}
}

void ConnectionManager::closeAll()
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Closing all connections" << std::endl;
#endif
	boost::recursive_mutex::scoped_lock lockClass(m_connectionManagerLock);
	for (Connection_ptr con : m_connections) {
		try {
			boost::system::error_code error;
			con->m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
			con->m_socket->close(error);
			}
		catch (boost::system::system_error&) {}
		}
	m_connections.clear();
}
