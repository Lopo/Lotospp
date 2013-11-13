#ifndef LOTOS2_ACCOUNT_H
#define	LOTOS2_ACCOUNT_H

#include <string>
#include <list>

#include <stdint.h>

struct Account
{
	Account() : number(0) {}
	~Account() {};

	uint32_t ip;
	uint16_t port;
	uint32_t number;
};

#endif	/* LOTOS2_ACCOUNT_H */
