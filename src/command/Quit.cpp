#include "command/Quit.h"


using namespace lotospp::command;


Quit::Quit()
	: Command("quit", enums::UserLevel_NOVICE)
{
}

void Quit::execute(lotospp::User* user, std::string inpstr)
{
	user->kick();
	user->removeList();
}
