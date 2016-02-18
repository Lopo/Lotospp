#ifndef LOTOSPP_COMMAND_H
#define	LOTOSPP_COMMAND_H


#include "config.h"

#include <string>

#include "AutoID.h"
#include "generated/enums.h"
#include "User.h"


namespace lotospp {

class Command
	: boost::noncopyable,
		public AutoID
{
public:
	Command(const std::string n="", const enums::UserLevel l=enums::UserLevel_NOVICE) : name(n), level(l) {};
	virtual ~Command() {};

	virtual void execute(User* user)=0;
	std::string getName();

private:
	std::string name;
	enums::UserLevel level=enums::UserLevel_NOVICE;
};

} // namespace lotospp

#endif // LOTOSPP_COMMAND_H
