#ifndef LOTOS2_NETWORK_SERVICEBASE_H
#define LOTOS2_NETWORK_SERVICEBASE_H


#include "config.h"

#include <stdint.h>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>


namespace lotos2 {
	namespace network {
		class Protocol;
		class Connection;
		typedef boost::shared_ptr<Connection> Connection_ptr;

class ServiceBase
	: boost::noncopyable
{
public:
	virtual ~ServiceBase() {}; // Redundant, but stifles compiler warnings

	virtual bool is_single_socket() const=0;
	virtual uint8_t get_protocol_identifier() const=0;
	virtual const char* get_protocol_name() const=0;

	virtual Protocol* make_protocol(Connection_ptr c) const=0;
};

typedef boost::shared_ptr<ServiceBase> Service_ptr;

	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_SERVICEBASE_H
