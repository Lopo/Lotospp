#ifndef LOTOS2_COMMAND_H
#define	LOTOS2_COMMAND_H


#include "config.h"

#include <string>

#include "AutoID.h"
#include "generated/enums.h"
#include "User.h"


namespace lotos2 {

class Command
	: boost::noncopyable,
		public AutoID
{
public:
	Command(const std::string n="", const enums::UserLevel l=enums::UserLevel_NOVICE) : name(n), level(l) {};
	virtual ~Command() {};

	virtual void execute(User* user, std::string inpstr)=0;
	std::string getName();

private:
	std::string name;
	enums::UserLevel level=enums::UserLevel_NOVICE;
};

} // namespace lotos2

#endif // LOTOS2_COMMAND_H
