#include "Say.h"


using namespace LotosPP::Commands;


Say::Say()
	: Command("say", LotosPP::Common::enums::UserLevel_NOVICE)
{
}

void Say::execute(LotosPP::Common::User* user)
{
	if (user->com.word.size()<2) {
		user->uPrintf("Usage: .say <text>\n");
		return;
		}
	user->uPrintf("You say: %s\n", user->com.wordptr(1).c_str());
	LotosPP::Common::User* u=nullptr;
	for (auto&& [f, second] : user->listUser.list) {
		u=second;
		if (u!=user) {
			u->uPrintf("%s say: %s\n", user->getName().c_str(), user->com.wordptr(1).c_str());
			}
		}
}
