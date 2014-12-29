#ifndef LOTOS2_NETWORK_PROTOCOL_CONST_H
#define LOTOS2_NETWORK_PROTOCOL_CONST_H

#include "config.h"

// This files defines limits of the protocol

// The maximum size of a single packet sent over the network
// This also indirectly limits the size of writeables etc.
#define NETWORKMESSAGE_MAXSIZE 15340

#endif /* LOTOS2_NETWORK_PROTOCOL_CONST_H */
