#include "ConnectionManager.h"
#include "Connection.h"
#include "Common/Singleton.h"
#include <iostream>


using namespace LotosPP::Network;
using namespace std;


ConnectionManager* ConnectionManager::getInstance()
{
	static LotosPP::Common::Singleton<ConnectionManager> instance;
	return instance.get();
}

Connection_ptr ConnectionManager::createConnection(boost::asio::ip::tcp::socket* socket, boost::asio::io_service& io_service, ServicePort_ptr servicer)
{
#ifdef __DEBUG_NET_DETAIL__
	cout << "Create new Connection" << endl;
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionManagerLock);
	Connection_ptr connection=boost::shared_ptr<Connection>(new Connection(socket, io_service, servicer));
	m_connections.push_back(connection);
	return connection;
}

void ConnectionManager::releaseConnection(Connection_ptr connection)
{
#ifdef __DEBUG_NET_DETAIL__
	cout << "Releasing connection" << endl;
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionManagerLock);
	if (list<Connection_ptr>::iterator it=find(m_connections.begin(), m_connections.end(), connection); it!=m_connections.end()) {
		m_connections.erase(it);
		}
	else {
		cout << "Error: [ConnectionManager::releaseConnection] Connection not found" << endl;
		}
}

void ConnectionManager::closeAll()
{
#ifdef __DEBUG_NET_DETAIL__
	cout << "Closing all connections" << endl;
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
