#ifndef LOTOSPP_MISC_H
#define LOTOSPP_MISC_H


#include "config.h"

#include <ctime>
#include <string>
#include <algorithm>


namespace lotospp {
	namespace strings {

inline void toLowerCaseString(std::string& source) { std::transform(source.begin(), source.end(), source.begin(), tolower);};

bool hasWhitespace(const std::string& str);
void formatTime(time_t time, char* buffer);
void terminate(std::string& str);
void removeFirst(std::string& str);

	} // namespace strings
} // namespace lotospp

#endif // LOTOSPP_MISC_H
