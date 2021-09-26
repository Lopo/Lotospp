#include "Quit.h"


using namespace LotosPP::Commands;


Quit::Quit()
	: Command("quit", LotosPP::Common::enums::UserLevel_NOVICE)
{
}

void Quit::execute(LotosPP::Common::User* user)
{
	user->kick();
	user->removeList();
}
