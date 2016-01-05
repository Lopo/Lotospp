#ifndef LOTOSPP_STRUCTS_CONNECTBLOCK_H
#define	LOTOSPP_STRUCTS_CONNECTBLOCK_H


#include "config.h"

#include <cstdint>


namespace lotospp {

struct ConnectBlock
{
	uint64_t startTime;
	uint64_t blockTime;
	uint32_t count;
};

} // namespace lotospp

#endif // LOTOSPP_STRUCTS_CONNECTBLOCK_H
