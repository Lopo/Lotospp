#ifndef LOTOS2_STRUCTS_CONNECTBLOCK_H
#define	LOTOS2_STRUCTS_CONNECTBLOCK_H

#include "config.h"

#include <stdint.h>


namespace lotos2 {

struct ConnectBlock
{
	uint64_t startTime;
	uint64_t blockTime;
	uint32_t count;
};

} // namespace lotos2

#endif /* LOTOS2_STRUCTS_CONNECTBLOCK_H */
