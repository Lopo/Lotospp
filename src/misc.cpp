#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#include <string>

#include <boost/algorithm/string.hpp>

#include "config.h"

using std::string;
using std::cout;

/*** Returns true if string has whitespace ***/
int has_whitespace(const char *str)
{
	for (; *str; ++str) {
		if (*str<33) {
			return 1;
			}
		}
	return 0;
}

void toLowerCaseString(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(), tolower);
}

//buffer should have at least 16 bytes
void formatTime(time_t time, char* buffer)
{
	const tm* tms=localtime(&time);
	if (tms) {
		strftime(buffer, 16, "%Y%m%dT%H%M%S", tms);
		}
	else {
		sprintf(buffer, "UNIX Time : %d", (int)time);
		}
}