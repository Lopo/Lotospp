#include "config.h"

#include <ctime>
#include <cstdio>
#include <string>

#include "misc.h"


namespace lotos2 {

/**
 * Returns true if string has whitespace
 */
bool hasWhitespace(const std::string& str)
{
	for (auto it=str.cbegin(); it!=str.cend(); ++it) {
		if (*it<33) {
			return true;
			}
		}
	return false;
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

} // namespace lotos2
