#ifndef LOTOSPP_COMMAND_QUIT_H
#define	LOTOSPP_COMMAND_QUIT_H


#include "config.h"

#include "Command.h"


namespace lotospp {
	namespace command {

class Quit
	: public Command
{
public:
	Quit();
	virtual void execute(lotospp::User* user);
};

	} // namespace command
} // namespace lotospp

#endif // LOTOSPP_COMMAND_QUIT_H
