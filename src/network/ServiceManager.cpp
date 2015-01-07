#include "config.h"

#include <stdint.h>

#if defined __WINDOWS__ || defined WIN32
#include <winerror.h>
#endif

#include <list>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/address.hpp>

#include "network/Service.h"
#include "Scheduler.h"
#include "network/OutputMessage.h"
#include "network/Connection.h"

#include "Logger.h"

#include "globals.h"

using namespace lotos2;
using lotos2::network::ServiceManager;


ServiceManager::ServiceManager()
	: m_io_service(), death_timer(m_io_service), running(false)
{
}

ServiceManager::~ServiceManager()
{
	stop();
}

std::list<uint16_t> ServiceManager::get_ports() const
{
	std::list<uint16_t> ports;
	for (std::map<uint16_t, ServicePort_ptr>::const_iterator it=m_acceptors.begin(); it!=m_acceptors.end(); ++it) {
		ports.push_back(it->first);
		}
	// Maps are ordered, so the elements are in order
	//ports.sort();
	ports.unique();
	return ports;
}

void ServiceManager::die()
{
	m_io_service.stop();
}

void ServiceManager::run()
{
	assert(!running);
	running=true;
	try {
		m_io_service.run();
		}
	catch (boost::system::system_error& e) {
		LOG_MESSAGE("NETWORK", LOGTYPE_ERROR, e.what());
		}
}

void ServiceManager::stop()
{
	if (!running) {
		return;
		}

	running=false;

	for (std::map<uint16_t, ServicePort_ptr>::iterator it=m_acceptors.begin(); it!=m_acceptors.end(); ++it) {
		try {
			m_io_service.post(boost::bind(&ServicePort::onStopServer, it->second));
			}
		catch (boost::system::system_error& e) {
			LOG_MESSAGE("NETWORK", LOGTYPE_ERROR, e.what());
			}
		}
	m_acceptors.clear();

	OutputMessagePool::getInstance()->stop();

	// Give the server 3 seconds to process all messages before death
	death_timer.expires_from_now(boost::posix_time::seconds(3));
	death_timer.async_wait(boost::bind(&ServiceManager::die, this));
}