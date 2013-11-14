#ifndef LOTOS2_MISC_H
#define LOTOS2_MISC_H

#include <time.h>

int has_whitespace(const char *str);
void toLowerCaseString(std::string& source);
void formatTime(time_t time, char* buffer);

#endif /* MISC_H */
