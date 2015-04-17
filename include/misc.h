#ifndef LOTOS2_MISC_H
#define LOTOS2_MISC_H


#include "config.h"

#include <ctime>
#include <string>
#include <algorithm>


namespace lotos2 {

bool hasWhitespace(const std::string& str);
inline void toLowerCaseString(std::string& source) { std::transform(source.begin(), source.end(), source.begin(), tolower);};
void formatTime(time_t time, char* buffer);

} // namespace lotos2

#endif // MISC_H
