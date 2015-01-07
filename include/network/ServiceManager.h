#ifndef LOTOS2_NETWORK_SERVICEMANAGER_H
#define LOTOS2_NETWORK_SERVICEMANAGER_H


#include "config.h"

#include <stdint.h>

#include <list>
#include <map>
#include <iostream>

#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "network/ServicePort.h"


namespace lotos2 {
	namespace network {

/**
 * The ServiceManager simply manages all services and handles startup/closing
 */
class ServiceManager
	: boost::noncopyable
{
	ServiceManager(const ServiceManager&);
public:
	ServiceManager();
	~ServiceManager();

	// Run and start all servers
	void run();
	void stop();

	// Adds a new service to be managed
	template <typename ProtocolType>
	bool add(uint16_t port);

	bool is_running() const { return m_acceptors.empty()==false;};
	std::list<uint16_t> get_ports() const;
protected:
	void die();

	std::map<uint16_t, ServicePort_ptr> m_acceptors;

	boost::asio::io_service m_io_service;
	boost::asio::deadline_timer death_timer;
	bool running;
};

template <typename ProtocolType>
bool ServiceManager::add(uint16_t port)
{
	if (!port) {
		std::cout << "NOTICE: No port provided for service " << ProtocolType::protocol_name() << ". Service disabled." << std::endl;
		return false;
		}
	ServicePort_ptr service_port;

	std::map<uint16_t, ServicePort_ptr>::iterator finder=m_acceptors.find(port);

	if (finder==m_acceptors.end()) {
		service_port.reset(new ServicePort(m_io_service));
		service_port->open(port);
		m_acceptors[port]=service_port;
		}
	else {
		service_port=finder->second;
		if (service_port->is_single_socket() || ProtocolType::server_sends_first) {
			std::cout << "ERROR: " << ProtocolType::protocol_name() <<
				" and " << service_port->get_protocol_names() <<
				" cannot use the same port " << port << "." << std::endl;
			return false;
			}
		}

	return service_port->add_service(Service_ptr(new Service<ProtocolType>()));
};

	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_SERVICEMANAGER_H
