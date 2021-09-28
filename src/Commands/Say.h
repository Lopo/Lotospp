#ifndef LOTOSPP_COMMANDS_SAY_H
#define	LOTOSPP_COMMANDS_SAY_H

#include "Common/Command.h"


namespace LotosPP::Commands {

class Say
	: public LotosPP::Common::Command
{
public:
	Say();
	virtual void execute(LotosPP::Common::User* user);
};

	}

#endif
