#ifndef LOTOS2_NETWORK_SERVICE_H
#define LOTOS2_NETWORK_SERVICE_H


#include "config.h"

#include <stdint.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "network/ServiceBase.h"
#include "network/Protocol.h"
#include "network/Connection.h"


namespace lotos2 {
	namespace network {

typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> Acceptor_ptr;

// The Service class is very thin, it's only real job is to create dynamic
// dispatch of the protocol attributes, which would otherwise be very hard,
// and require templating of many Service functions

template <typename ProtocolType>
class Service
	: public ServiceBase
{
public:
	bool is_single_socket() const { return ProtocolType::server_sends_first;};
	uint8_t get_protocol_identifier() const { return ProtocolType::protocol_identifier;};
	const char* get_protocol_name() const { return ProtocolType::protocol_name();};

	Protocol* make_protocol(Connection_ptr c) const { return new ProtocolType(c);};
};

	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_SERVICE_H
