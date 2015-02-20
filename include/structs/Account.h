#ifndef LOTOS2_STRUCTS_ACCOUNT_H
#define	LOTOS2_STRUCTS_ACCOUNT_H


#include "config.h"

#include <cstdint>


namespace lotos2 {

struct Account
{
	Account() {};
	~Account() {};

	uint32_t ip;
	uint16_t port;
	uint32_t number=0;
	uint32_t warnings=0;
};

} // namespace lotos2

#endif // LOTOS2_STRUCTS_ACCOUNT_H
