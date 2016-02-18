#ifndef LOTOSPP_COMMAND_SAY_H
#define	LOTOSPP_COMMAND_SAY_H


#include "config.h"

#include "Command.h"


namespace lotospp {
	namespace command {

class Say
	: public Command
{
public:
	Say();
	virtual void execute(lotospp::User* user);
};

	} // namespace command
} // namespace lotospp

#endif // LOTOSPP_COMMAND_SAY_H
