#include "Commands/Quit.h"


using namespace lotospp::command;


Quit::Quit()
	: Command("quit", enums::UserLevel_NOVICE)
{
}

void Quit::execute(lotospp::User* user)
{
	user->kick();
	user->removeList();
}
