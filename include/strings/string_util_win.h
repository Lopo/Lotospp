#ifndef LOTOS2_STRINGS_STRING_UTIL_WIN_H
#define LOTOS2_STRINGS_STRING_UTIL_WIN_H


#include <cstdarg>
#include <cstdio>
#include <cstring>


namespace lotos2 {
	namespace strings {

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

	} // namespace strings
} // namespace lotos2

#endif // LOTOS2_STRINGS_STRING_UTIL_WIN_H
