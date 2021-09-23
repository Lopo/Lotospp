#ifndef LOTOSPP_STRUCTS_ACCOUNT_H
#define	LOTOSPP_STRUCTS_ACCOUNT_H


#include "config.h"

#include <cstdint>


namespace lotospp {

struct Account
{
	Account() {};
	~Account() {};

	uint32_t ip;
	uint16_t port;
	uint32_t number=0;
	uint32_t warnings=0;
};

} // namespace lotospp

#endif // LOTOSPP_STRUCTS_ACCOUNT_H
