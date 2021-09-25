#ifndef LOTOSPP_STRINGS_STRING_UTIL_POSIX_H
#define LOTOSPP_STRINGS_STRING_UTIL_POSIX_H

#include <cstdio>
#include <cstdarg>


namespace LotosPP::Strings {

inline int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments)
{
	return ::vsnprintf(buffer, size, format, arguments);
}

	}

#endif
