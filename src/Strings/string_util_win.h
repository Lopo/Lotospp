#ifndef LOTOSPP_STRINGS_STRING_UTIL_WIN_H
#define LOTOSPP_STRINGS_STRING_UTIL_WIN_H

#include <cstdarg>
#include <cstdio>
#include <cstring>


namespace LotosPP::Strings {

inline int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments)
{
	int length=_vsprintf_p(buffer, size, format, arguments);
	if (length<0) {
		if (size>0) {
			buffer[0]=0;
			}
		return _vscprintf_p(format, arguments);
		}
	return length;
}

	}

#endif
