#ifndef LOTOS2_MISC_H
#define LOTOS2_MISC_H


#include "config.h"

#include <time.h>

#include <string>


namespace lotos2 {

int has_whitespace(const char *str);
void toLowerCaseString(std::string& source);
void formatTime(time_t time, char* buffer);

} // namespace lotos2

#endif // MISC_H
