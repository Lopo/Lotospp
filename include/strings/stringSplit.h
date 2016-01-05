#ifndef LOTOS2_STRINGS_STRINGSPLIT_H
#define	LOTOS2_STRINGS_STRINGSPLIT_H

#include "config.h"

#include <string>
#include <vector>


namespace lotos2 {
	namespace strings {

std::vector<std::string> &StringSplit(const std::string& s, const std::string& delim, std::vector<std::string> &elems);
std::vector<std::string> StringSplit(const std::string& s, const std::string& delim);
std::vector<std::string> wordFind(const std::string& inpstr);

	} // namespace strings
} // namespace lotos2

#endif // LOTOS2_STRINGS_STRINGSPLIT_H
