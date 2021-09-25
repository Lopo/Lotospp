#include "Commands/Say.h"


using namespace lotospp::command;


Say::Say()
	: Command("say", enums::UserLevel_NOVICE)
{
}

void Say::execute(lotospp::User* user)
{
	if (user->com.word.size()<2) {
		user->uPrintf("Usage: .say <text>\n");
		return;
		}
	user->uPrintf("You say: %s\n", user->com.wordptr(1).c_str());
	User* u=nullptr;
	for (auto&& [f, second] : user->listUser.list) {
		u=second;
		if (u!=user) {
			u->uPrintf("%s say: %s\n", user->getName().c_str(), user->com.wordptr(1).c_str());
			}
		}
}
