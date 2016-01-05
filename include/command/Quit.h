#ifndef LOTOS2_COMMAND_QUIT_H
#define	LOTOS2_COMMAND_QUIT_H


#include "config.h"

#include "Command.h"


namespace lotos2 {
	namespace command {

class Quit
	: public Command
{
public:
	Quit();
	virtual void execute(lotos2::User* user, std::string inpstr);
};

	} // namespace command
} // namespace lotos2

#endif // LOTOS2_COMMAND_QUIT_H
