#include "ServicePort.h"
#include "ServiceBase.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include "Log/Logger.h"
#include "globals.h"
#include "System/build_config.h"
#include <boost/bind/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ip/v6_only.hpp>
#if defined(__DEBUG_NET__) || defined(__DEBUG_NET_DETAIL__)
#	include <iostream>
#endif
#ifdef OS_WIN
#	include <winerror.h>
#endif


using namespace LotosPP::Network;
using namespace std;


///////////////////////////////////////////////////////////////////////////////
// ServicePort

ServicePort::ServicePort(boost::asio::io_service& io_service)
	: m_io_service{io_service}, m_serverPort{0}, m_pendingStart{false}
{
}

ServicePort::~ServicePort()
{
	close();
}

bool ServicePort::isSingleSocket() const
{
	return m_services.size() && m_services.front()->isSingleSocket();
}

std::string ServicePort::getProtocolNames() const
{
	if (m_services.empty()) {
		return "";
		}
	string str=m_services.front()->getProtocolName();
	for (uint32_t i=1; i<m_services.size(); ++i) {
		str+=", ";
		str+=m_services[i]->getProtocolName();
		}
	return str;
}

void ServicePort::accept(Acceptor_ptr acceptor)
{
	try {
		boost::asio::ip::tcp::socket* socket=new boost::asio::ip::tcp::socket(m_io_service);

		acceptor->async_accept(
			*socket,
			boost::bind(
				&ServicePort::onAccept,
				this,
				acceptor,
				socket,
				boost::asio::placeholders::error
				)
			);
		}
	catch (boost::system::system_error& e) {
		if (m_logError) {
			LOG(LERROR) << e.what();
			m_logError=false;
            }
        }
}

void ServicePort::onAccept(Acceptor_ptr acceptor, boost::asio::ip::tcp::socket* socket, const boost::system::error_code& error)
{
	if (!error) {
		if (m_services.empty()) {
#ifdef __DEBUG_NET__
			cout << "Error: [ServerPort::accept] No services running!" << endl;
#endif
			return;
		}

		boost::system::error_code error;
		const boost::asio::ip::tcp::endpoint endpoint=socket->remote_endpoint(error);
		boost::asio::ip::address remote_ip=boost::asio::ip::address();
		if (!error) {
			remote_ip=endpoint.address();
			}

		if (!remote_ip.is_unspecified()) {
			Connection_ptr connection=ConnectionManager::getInstance()->createConnection(socket, m_io_service, shared_from_this());

			if (m_services.front()->isSingleSocket()) {
				// Only one handler, and it will send first
				connection->acceptConnection(m_services.front()->makeProtocol(connection));
				}
			else {
				connection->acceptConnection();
				}
			}
		else {
			//close the socket
			if (socket->is_open()) {
				boost::system::error_code error;
				socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
				socket->close(error);
				delete socket;
				}
			}

#ifdef __DEBUG_NET_DETAIL__
		cout << "accept - OK" << endl;
#endif
		accept(acceptor);
		}
	else {
		if (error!=boost::asio::error::operation_aborted) {
			close();

			if (!m_pendingStart) {
				m_pendingStart=true;
				g_scheduler.addEvent(LotosPP::Common::createSchedulerTask(
						5000,
						boost::bind(
							&ServicePort::openAcceptor,
							boost::weak_ptr<ServicePort>(shared_from_this()),
							m_serverPort
							)
					));
				}
			}
		else {
#ifdef __DEBUG_NET__
			cout << "Error: [ServerPort::onAccept] Operation aborted." << endl;
#endif
			}
		}
}

Protocol* ServicePort::makeProtocol(NetworkMessage& msg) const
{
	for (Service_ptr service : m_services) {
		return service->makeProtocol(Connection_ptr());
		// We can ignore the other cases, they will most likely end up in return NULL anyways.
		}

	return nullptr;
}

void ServicePort::onStopServer()
{
	close();
}

void ServicePort::openAcceptor(boost::weak_ptr<ServicePort> weak_service, uint16_t port)
{
	if (weak_service.expired()) {
		return;
		}

	if (ServicePort_ptr service=weak_service.lock()) {
#ifdef __DEBUG_NET_DETAIL__
		cout << "ServicePort::openAcceptor" << endl;
#endif
		service->open(port);
		}
}

void ServicePort::open(uint16_t port)
{
	namespace ip=boost::asio::ip;

	m_serverPort=port;
	m_pendingStart=false;

	try {
#ifdef ENABLE_IPV6
		ip::v6_only v6_only;
		boost::system::error_code ec;
		Acceptor_ptr aptr(new ip::tcp::acceptor(m_io_service, ip::tcp::endpoint(ip::address_v6(), m_serverPort)));
		aptr->set_option(v6_only, ec);
		aptr->set_option(ip::tcp::no_delay(true));
		aptr->get_option(v6_only);
		accept(aptr);
		m_tcp_acceptors.push_back(aptr);
		if (!aptr->is_open() || v6_only) {
#endif
			Acceptor_ptr aptr(new ip::tcp::acceptor(m_io_service, ip::tcp::endpoint(ip::address(), m_serverPort)));
			aptr->set_option(ip::tcp::no_delay(true));
			accept(aptr);
			m_tcp_acceptors.push_back(aptr);
#ifdef ENABLE_IPV6
			}
#endif
		}
	catch (boost::system::system_error& e) {
		if (m_logError) {
			LOG(LERROR) << e.what();
			m_logError=false;
			}

		m_pendingStart=true;
		g_scheduler.addEvent(LotosPP::Common::createSchedulerTask(
				5000,
				boost::bind(
					&ServicePort::openAcceptor,
					boost::weak_ptr<ServicePort>(shared_from_this()),
					port
					)
				));
		}
}

void ServicePort::close()
{
	for (Acceptor_ptr aptr : m_tcp_acceptors) {
		if (aptr->is_open()) {
			boost::system::error_code error;
			aptr->close(error);
			if (error) {
				PRINT_ASIO_ERROR("Closing listen socket");
				}
			}
		}
	m_tcp_acceptors.clear();
}

bool ServicePort::addService(Service_ptr new_svc)
{
	for (Service_ptr svc : m_services) {
		if (svc->isSingleSocket()) {
			return false;
			}
		}

	m_services.push_back(new_svc);
	return true;
}
