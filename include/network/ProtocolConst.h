#ifndef LOTOSPP_NETWORK_PROTOCOLCONST_H
#define LOTOSPP_NETWORK_PROTOCOLCONST_H


#include "config.h"


namespace lotospp {
	namespace network {

// This files defines limits of the protocol

// The maximum size of a single packet sent over the network
// This also indirectly limits the size of writeables etc.
#define NETWORKMESSAGE_MAXSIZE 15340

	} // namespace network
} // namespace lotospp

#endif // LOTOSPP_NETWORK_PROTOCOLCONST_H
