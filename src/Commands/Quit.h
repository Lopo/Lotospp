#ifndef LOTOSPP_COMMANDS_QUIT_H
#define	LOTOSPP_COMMANDS_QUIT_H

#include "Common/Command.h"


namespace LotosPP::Commands {

class Quit
	: public LotosPP::Common::Command
{
public:
	Quit();
	virtual void execute(LotosPP::Common::User* user);
};

	}

#endif
