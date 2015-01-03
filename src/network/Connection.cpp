#include "config.h"

#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include "Singleton.h"
#include "network/Connection.h"
#include "network/Protocol.h"
#include "network/OutputMessage.h"
#include "Scheduler.h"
#include "server.h"
#include "Logger.h"


using namespace lotos2;
using lotos2::network::Connection;


bool Connection::m_logError=true;

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t Connection::connectionCount=0;
#endif

network::ConnectionManager* network::ConnectionManager::getInstance()
{
	static Singleton<network::ConnectionManager> instance;
	return instance.get();
}

network::Connection_ptr network::ConnectionManager::createConnection(boost::asio::ip::tcp::socket* socket, boost::asio::io_service& io_service, ServicePort_ptr servicer)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Create new Connection" << std::endl;
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionManagerLock);
	Connection_ptr connection=boost::shared_ptr<Connection>(new Connection(socket, io_service, servicer));
	m_connections.push_back(connection);
	return connection;
}

void network::ConnectionManager::releaseConnection(Connection_ptr connection)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Releasing connection" << std::endl;
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionManagerLock);
	std::list<Connection_ptr>::iterator it=std::find(m_connections.begin(), m_connections.end(), connection);

	if (it!=m_connections.end()) {
		m_connections.erase(it);
		}
	else {
		std::cout << "Error: [ConnectionManager::releaseConnection] Connection not found" << std::endl;
		}
}

void network::ConnectionManager::closeAll()
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Closing all connections" << std::endl;
#endif
	boost::recursive_mutex::scoped_lock lockClass(m_connectionManagerLock);
	std::list<Connection_ptr>::iterator it;
	for (it=m_connections.begin(); it!=m_connections.end(); ) {
		try {
			boost::system::error_code error;
			(*it)->m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
			(*it)->m_socket->close(error);
			}
		catch (boost::system::system_error&) {
			}
		++it;
		}
	m_connections.clear();
}

Connection::Connection(boost::asio::ip::tcp::socket* socket, boost::asio::io_service& io_service, ServicePort_ptr service_port)
	: m_socket(socket),
		m_writeTimer(io_service),
		m_io_service(io_service),
		m_service_port(service_port)
{
	m_refCount=0;
	m_protocol=NULL;
	m_pendingWrite=0;
	m_pendingRead=0;
	m_connectionState=CONNECTION_STATE_OPEN;
	m_receivedFirst=false;
	m_writeError=false;
	m_readError=false;

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	connectionCount++;
#endif
}

Connection::~Connection()
{
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	connectionCount--;
#endif
}

boost::asio::ip::tcp::socket& Connection::getHandle()
{
	return *m_socket;
}

void Connection::closeConnection()
{
	//any thread
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Connection::closeConnection" << std::endl;
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionLock);
	if (m_connectionState==CONNECTION_STATE_CLOSED || m_connectionState==CONNECTION_STATE_REQUEST_CLOSE) {
		return;
		}

	m_connectionState=CONNECTION_STATE_REQUEST_CLOSE;

	g_dispatcher.addTask(createTask(boost::bind(&Connection::closeConnectionTask, this)));
}

void Connection::closeConnectionTask()
{
	//dispatcher thread
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Connection::closeConnectionTask" << std::endl;
#endif

	m_connectionLock.lock();
	if (m_connectionState!=CONNECTION_STATE_REQUEST_CLOSE) {
		std::cout << "Error: [Connection::closeConnectionTask] m_connectionState = " << m_connectionState << std::endl;
		m_connectionLock.unlock();
		return;
		}

	if (m_protocol) {
		m_protocol->setConnection(Connection_ptr());
		m_protocol->releaseProtocol();
		m_protocol=NULL;
		}

	m_connectionState=CONNECTION_STATE_CLOSING;

	if (m_pendingWrite==0 || m_writeError) {
		closeSocket();
		releaseConnection();
		m_connectionState=CONNECTION_STATE_CLOSED;
		}
	else {
		//will be closed by onWriteOperation/handleWriteTimeout/handleReadTimeout instead
		}

	m_connectionLock.unlock();
}

void Connection::closeSocket()
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Connection::closeSocket" << std::endl;
#endif

	m_connectionLock.lock();

	if (m_socket->is_open()) {
#ifdef __DEBUG_NET_DETAIL__
		std::cout << "Closing socket" << std::endl;
#endif

		m_pendingRead=0;
		m_pendingWrite=0;

		try {
			boost::system::error_code error;
			m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
			if (error) {
				if (error==boost::asio::error::not_connected) {
					//Transport endpoint is not connected.
					}
				else {
					PRINT_ASIO_ERROR("Shutdown");
					}
				}
			m_socket->close(error);

			if (error) {
				PRINT_ASIO_ERROR("Close");
				}
			}
		catch (boost::system::system_error& e) {
			if (m_logError) {
				LOG_MESSAGE("NETWORK", LOGTYPE_ERROR, e.what());
				m_logError=false;
				}
			}
		}

	m_connectionLock.unlock();
}

void Connection::releaseConnection()
{
	if (m_refCount>0) {
		//Reschedule it and try again.
		g_scheduler.addEvent(createSchedulerTask(SCHEDULER_MINTICKS, boost::bind(&Connection::releaseConnection, this)));
		}
	else {
		deleteConnectionTask();
		}
}

void Connection::onStopOperation()
{
	//io_service thread
	m_connectionLock.lock();
	m_writeTimer.cancel();

	try {
		if (m_socket->is_open()) {
			boost::system::error_code error;
			m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
			m_socket->close();
			}
		}
	catch (boost::system::system_error&) {
		//
		}

	delete m_socket;
	m_socket=NULL;

	m_connectionLock.unlock();
	ConnectionManager::getInstance()->releaseConnection(shared_from_this());
}

void Connection::deleteConnectionTask()
{
	//dispather thread
	assert(m_refCount==0);
	try {
		m_io_service.dispatch(boost::bind(&Connection::onStopOperation, this));
		}
	catch (boost::system::system_error& e) {
		if (m_logError) {
			LOG_MESSAGE("NETWORK", LOGTYPE_ERROR, e.what());
			m_logError=false;
			}
		}
}

void Connection::acceptConnection(Protocol* protocol)
{
	m_protocol=protocol;
	m_protocol->onConnect();

	acceptConnection();
}

void Connection::acceptConnection()
{
	try {
		++m_pendingRead;

		// Read size of the first packet
		getHandle().async_read_some(
			boost::asio::buffer(m_msg.getBuffer(), NetworkMessage::max_body_length),
			boost::bind(&Connection::parsePacket, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
			);
		}
	catch (boost::system::system_error& e) {
		if (m_logError) {
			LOG_MESSAGE("NETWORK", LOGTYPE_ERROR, e.what());
			m_logError=false;
			closeConnection();
			}
		}
}

void Connection::parsePacket(const boost::system::error_code& error, const std::size_t bytes_transferred)
{
	m_connectionLock.lock();

	if (error) {
		handleReadError(error);
		}

	if (m_connectionState!=CONNECTION_STATE_OPEN || m_readError) {
		closeConnection();
		m_connectionLock.unlock();
		return;
		}

	--m_pendingRead;

	m_msg.setMessageLength(bytes_transferred);

	if (!m_receivedFirst) {
		m_receivedFirst=true;
		// First message received
		if (!m_protocol) { // protocol has already been created at this point
			m_protocol=m_service_port->make_protocol(m_msg);
			if (!m_protocol) {
				closeConnection();
				m_connectionLock.unlock();
				return;
				}
			m_protocol->setConnection(shared_from_this());
			}
		else {
			// Skip protocol ID
//			m_msg.GetByte();
			}
		m_protocol->onRecvFirstMessage(m_msg);
		}
	else {
		// Send the packet to the current protocol
		m_protocol->onRecvMessage(m_msg);
		}

	try {
		++m_pendingRead;

		// Wait to the next packet
		getHandle().async_read_some(
			boost::asio::buffer(m_msg.getBuffer(), NetworkMessage::max_body_length),
			boost::bind(&Connection::parsePacket, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
			);
		}
	catch (boost::system::system_error& e) {
		if (m_logError) {
			LOG_MESSAGE("NETWORK", LOGTYPE_ERROR, e.what());
			m_logError=false;
			closeConnection();
			}
		}

	m_connectionLock.unlock();
}

bool Connection::send(OutputMessage_ptr msg)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Connection::send init" << std::endl;
#endif

	m_connectionLock.lock();
	if (m_connectionState!=CONNECTION_STATE_OPEN || m_writeError) {
		m_connectionLock.unlock();
		return false;
		}

	if (m_pendingWrite==0) {
		msg->getProtocol()->onSendMessage(msg);

		TRACK_MESSAGE(msg);

#ifdef __DEBUG_NET_DETAIL__
		std::cout << "Connection::send " << msg->getMessageLength() << std::endl;
#endif

		internalSend(msg);
		}
	else {
#ifdef __DEBUG_NET__
		std::cout << "Connection::send Adding to queue " << msg->getMessageLength() << std::endl;
#endif

		TRACK_MESSAGE(msg);
		OutputMessagePool* outputPool=OutputMessagePool::getInstance();
		outputPool->addToAutoSend(msg);
		}

	m_connectionLock.unlock();
	return true;
}

void Connection::internalSend(OutputMessage_ptr msg)
{
	TRACK_MESSAGE(msg);

	try {
		++m_pendingWrite;
		m_writeTimer.expires_from_now(boost::posix_time::seconds(Connection::write_timeout));
		m_writeTimer.async_wait(boost::bind(
				&Connection::handleWriteTimeout,
				boost::weak_ptr<Connection>(shared_from_this()),
				boost::asio::placeholders::error
			));

		boost::asio::async_write(
			getHandle(),
			boost::asio::buffer(msg->getOutputBuffer(), msg->getMessageLength()),
			boost::bind(&Connection::onWriteOperation, shared_from_this(), msg, boost::asio::placeholders::error)
			);
		}
	catch (boost::system::system_error& e) {
		if (m_logError) {
			LOG_MESSAGE("NETWORK", LOGTYPE_ERROR, e.what());
			m_logError=false;
			}
		}
}

boost::asio::ip::address Connection::getIP() const
{
	//Ip is expressed in network byte order
	boost::system::error_code error;
	const boost::asio::ip::tcp::endpoint endpoint=m_socket->remote_endpoint(error);
	if (!error) {
		return endpoint.address();
		}
	PRINT_ASIO_ERROR("Getting remote ip");
	return boost::asio::ip::address();
}

int32_t Connection::addRef()
{
	return ++m_refCount;
}

int32_t Connection::unRef()
{
	return --m_refCount;
}

void Connection::onWriteOperation(OutputMessage_ptr msg, const boost::system::error_code& error)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "onWriteOperation" << std::endl;
#endif

	m_connectionLock.lock();
	m_writeTimer.cancel();

	TRACK_MESSAGE(msg);
	msg.reset();

	if (error) {
		handleWriteError(error);
		}

	if (m_connectionState!=CONNECTION_STATE_OPEN || m_writeError) {
		closeSocket();
		closeConnection();
		m_connectionLock.unlock();
		return;
		}

	--m_pendingWrite;
	m_connectionLock.unlock();
}

void Connection::handleReadError(const boost::system::error_code& error)
{
#ifdef __DEBUG_NET_DETAIL__
	PRINT_ASIO_ERROR("Reading - detail");
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionLock);

	if (error==boost::asio::error::operation_aborted) {
		//Operation aborted because connection will be closed
		//Do NOT call closeConnection() from here
		}
	else if (error==boost::asio::error::eof) {
		//No more to read
		closeConnection();
		}
	else if (error==boost::asio::error::connection_reset || error==boost::asio::error::connection_aborted) {
		//Connection closed remotely
		closeConnection();
		}
	else {
		PRINT_ASIO_ERROR("Reading");
		closeConnection();
		}
	m_readError=true;
}

void Connection::onReadTimeout()
{
	boost::recursive_mutex::scoped_lock lockClass(m_connectionLock);

	if (m_pendingRead>0 || m_readError) {
		closeSocket();
		closeConnection();
		}
}

void Connection::onWriteTimeout()
{
	boost::recursive_mutex::scoped_lock lockClass(m_connectionLock);

	if (m_pendingWrite>0 || m_writeError) {
		closeSocket();
		closeConnection();
		}
}

void Connection::handleReadTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error)
{
	if (error!=boost::asio::error::operation_aborted) {
		if (weak_conn.expired()) {
			return;
			}

		if (boost::shared_ptr<Connection> connection=weak_conn.lock()) {
#ifdef __DEBUG_NET_DETAIL__
			std::cout << "Connection::handleReadTimeout" << std::endl;
#endif
			connection->onReadTimeout();
			}
		}
}

void Connection::handleWriteError(const boost::system::error_code& error)
{
#ifdef __DEBUG_NET_DETAIL__
	PRINT_ASIO_ERROR("Writing - detail");
#endif

	boost::recursive_mutex::scoped_lock lockClass(m_connectionLock);

	if (error==boost::asio::error::operation_aborted) {
		//Operation aborted because connection will be closed
		//Do NOT call closeConnection() from here
		}
	else if (error==boost::asio::error::eof) {
		//No more to read
		closeConnection();
		}
	else if (error==boost::asio::error::connection_reset || error==boost::asio::error::connection_aborted) {
		//Connection closed remotely
		closeConnection();
		}
	else {
		PRINT_ASIO_ERROR("Writing");
		closeConnection();
		}
	m_writeError=true;
}

void Connection::handleWriteTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error)
{
	if (error!=boost::asio::error::operation_aborted) {
		if (weak_conn.expired()) {
			return;
			}

		if (boost::shared_ptr<Connection> connection=weak_conn.lock()) {
#ifdef __DEBUG_NET_DETAIL__
			std::cout << "Connection::handleWriteTimeout" << std::endl;
#endif
			connection->onWriteTimeout();
			}
		}
}
