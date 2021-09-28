#ifndef LOTOSPP_STRINGS_MISC_H
#define LOTOSPP_STRINGS_MISC_H

#include <string>
#include <algorithm>
#include <ctime>


namespace LotosPP::Strings {

inline void toLowerCaseString(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(), tolower);
}

bool hasWhitespace(const std::string& str);
void formatTime(time_t time, char* buffer);
void terminate(std::string& str);
void removeFirst(std::string& str);
void cleanString(std::string& str);

	}

#endif
