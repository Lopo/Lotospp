#ifndef LOTOS2_NETWORK_PROTOCOL_CONST_H
#define LOTOS2_NETWORK_PROTOCOL_CONST_H

#include "config.h"

namespace lotos2 {
	namespace network {

// This files defines limits of the protocol

// The maximum size of a single packet sent over the network
// This also indirectly limits the size of writeables etc.
#define NETWORKMESSAGE_MAXSIZE 15340

	} // namespace network
} // namespace lotos2

#endif /* LOTOS2_NETWORK_PROTOCOL_CONST_H */
