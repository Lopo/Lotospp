#include "misc.h"

#include <cstdio>


namespace lotospp {

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

/*** Put string terminate char. at first char < 32 ***/
void terminate(std::string& str)
{
	for (auto it=str.begin(); it!=str.end(); ++it) {
		if (*it<32) {
			str.erase(it, str.end());
			return;
			}
		}
}

void removeFirst(std::string& str)
{
	auto it=str.begin();
	while (*it<33 && *it) {
		++it;
		}
	while (*it>32) {
		++it;
		}
	while (*it<33 && *it) {
		++it;
		}
	str.erase(str.begin(), it);
}

} // namespace lotospp
