#include "config.h"

#include "command/Quit.h"


using namespace lotos2::command;


Quit::Quit()
	: Command("quit", enums::UserLevel_NOVICE)
{
}

void Quit::execute(lotos2::User* user, std::string inpstr)
{
	user->kick();
	user->removeList();
}
