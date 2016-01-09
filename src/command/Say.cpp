#include "command/Say.h"


using namespace lotospp::command;


Say::Say()
	: Command("say", enums::UserLevel_NOVICE)
{
}

void Say::execute(lotospp::User* user, std::string inpstr)
{
	if (user->getWords().size()<2) {
		user->uWrite("Usage: .say <text>\n");
		return;
		}
	user->uPrintf("You say: %s\n", inpstr.c_str());
	User* u=nullptr;
	for (auto it=user->listUser.list.begin(); it!=user->listUser.list.end(); it++) {
		u=(*it).second;
		if (u!=user) {
			u->uPrintf("%s say: %s\n", user->getName().c_str(), inpstr.c_str());
			}
		}
}
