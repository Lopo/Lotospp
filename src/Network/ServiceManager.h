#ifndef LOTOSPP_NETWORK_SERVICEMANAGER_H
#define LOTOSPP_NETWORK_SERVICEMANAGER_H

#include "ServicePort.h"
#include "Service.h"
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <list>
#include <map>
#include <iostream>
#include <cstdint>


namespace LotosPP::Network {

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
	// Remove service
	bool remove(uint16_t port);

	bool isRunning() const
	{
		return !m_acceptors.empty();
	};
	std::list<uint16_t> getPorts() const;
protected:
	void die();

	std::map<uint16_t, ServicePort_ptr> m_acceptors{};

	boost::asio::io_service m_io_service;
	boost::asio::deadline_timer death_timer;
	bool running{false};
};


template <typename ProtocolType>
bool ServiceManager::add(uint16_t port)
{
	if (!port) {
		std::cout << "NOTICE: No port provided for service " << ProtocolType::protocolName() << ". Service disabled." << std::endl;
		return false;
		}
	ServicePort_ptr service_port{nullptr};

	std::map<uint16_t, ServicePort_ptr>::iterator finder=m_acceptors.find(port);

	if (finder==m_acceptors.end()) {
		service_port.reset(new ServicePort(m_io_service));
		service_port->open(port);
		m_acceptors[port]=service_port;
		}
	else {
		service_port=finder->second;
		if (service_port->isSingleSocket() || ProtocolType::server_sends_first) {
			std::cout << "ERROR: " << ProtocolType::protocolName()
				<< " and " << service_port->getProtocolNames()
				<< " cannot use the same port " << port << "." << std::endl;
			return false;
			}
		}

	return service_port->addService(Service_ptr(new Service<ProtocolType>()));
}

	}

#endif
