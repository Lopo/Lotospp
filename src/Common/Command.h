#ifndef LOTOSPP_COMMON_COMMAND_H
#define	LOTOSPP_COMMON_COMMAND_H


#include "config.h"
#include "AutoID.h"
#include "Common/Enums/UserLevel.h"
#include "User.h"
#include <string>


namespace LotosPP::Common {

class Command
	: boost::noncopyable,
		public AutoID
{
public:
	Command(const std::string n="", const enums::UserLevel l=enums::UserLevel_NOVICE)
		: name(n), level(l)
	{};
	virtual ~Command()
	{};

	virtual void execute(User* user)=0;
	std::string getName();

private:
	std::string name;
	enums::UserLevel level{enums::UserLevel_NOVICE};
};

	}

#endif
