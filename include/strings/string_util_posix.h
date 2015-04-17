#ifndef LOTOS2_STRINGS_STRING_UTIL_POSIX_H
#define LOTOS2_STRINGS_STRING_UTIL_POSIX_H


#include <cstdio>
#include <cstdarg>


namespace lotos2 {
	namespace strings {

inline int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments)
{
	return ::vsnprintf(buffer, size, format, arguments);
}

	} // namespace strings
} // namespace lotos2

#endif // LOTOS2_STRINGS_STRING_UTIL_POSIX_H
