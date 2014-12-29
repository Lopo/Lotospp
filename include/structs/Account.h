#ifndef LOTOS2_STRUCTS_ACCOUNT_H
#define	LOTOS2_STRUCTS_ACCOUNT_H

#include "config.h"

#include <string>
#include <list>

#include <stdint.h>

struct Account
{
	Account() : number(0), warnings(0) {}
	~Account() {};

	uint32_t ip;
	uint16_t port;
	uint32_t number;
	uint32_t warnings;
};

#endif /* LOTOS2_STRUCTS_ACCOUNT_H */
