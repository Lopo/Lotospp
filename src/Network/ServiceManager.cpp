#include "ServiceManager.h"
#include "OutputMessage.h"
#include "Log/Logger.h"
#include "System/build_config.h"
#include <cassert>
#ifdef OS_WIN
#	include <winerror.h>
#endif


using namespace LotosPP::Network;
using namespace std;


ServiceManager::ServiceManager()
	: m_io_service(), death_timer{m_io_service}
{
}

ServiceManager::~ServiceManager()
{
	stop();
}

std::list<uint16_t> ServiceManager::getPorts() const
{
	list<uint16_t> ports{};
	for (const auto& [f, s] : m_acceptors) {
		ports.push_back(f);
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
		LOG(LERROR) << e.what();
		}
}

void ServiceManager::stop()
{
	if (!running) {
		return;
		}

	running=false;

	for (const auto& [f, s] : m_acceptors) {
		try {
			m_io_service.post(boost::bind(&ServicePort::onStopServer, s));
			}
		catch (boost::system::system_error& e) {
			LOG(LERROR) << e.what();
			}
		}
	m_acceptors.clear();

	OutputMessagePool::getInstance()->stop();

	// Give the server 3 seconds to process all messages before death
	death_timer.expires_from_now(boost::posix_time::seconds(3));
	death_timer.async_wait(boost::bind(&ServiceManager::die, this));
}

bool ServiceManager::remove(uint16_t port)
{
	if (!port) {
		cout << "NOTICE: No port provided for service remove. Service not removed." << endl;
		return false;
		}

	map<uint16_t, ServicePort_ptr>::iterator finder=m_acceptors.find(port);
	if (finder==m_acceptors.end()) {
		cout << "ERROR: " << "No service found for port " << port;
		return false;
		}
	ServicePort_ptr service_port=finder->second;
	service_port->close();
	m_acceptors.erase(finder);
	delete service_port.get();
	return true;
}
