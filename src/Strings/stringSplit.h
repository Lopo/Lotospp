#ifndef LOTOSPP_STRINGS_STRINGSPLIT_H
#define	LOTOSPP_STRINGS_STRINGSPLIT_H

#include "config.h"
#include <string>
#include <vector>


namespace LotosPP::Strings {

std::vector<std::string>& StringSplit(const std::string& s, const std::string& delim, std::vector<std::string>& elems);
std::vector<std::string> StringSplit(const std::string& s, const std::string& delim);
std::vector<std::string> wordFind(const std::string& inpstr);

	}

#endif
