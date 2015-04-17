#ifndef LOTOS2_STRINGS_STRING_UTIL_H
#define LOTOS2_STRINGS_STRING_UTIL_H


#include <cctype>
#include <cstdarg> // va_list
#include <string>
#include <vector>

#include "macros.h"
#include "system/port.h"
#include "system/compiler_specific.h"


namespace lotos2 {
	namespace strings {

// C standard-library functions like "strncasecmp" and "snprintf" that aren't
// cross-platform are provided as "strings::strncasecmp", and their prototypes
// are listed below. These functions are then implemented as inline calls
// to the platform-specific equivalents in the platform-specific headers.

// Wrapper for vsnprintf that always null-terminates and always returns the
// number of characters that would be in an untruncated formatted
// string, even when truncation occurs.
int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments)
	PRINTF_FORMAT(3, 0);

	} // namespace strings
} // namespace lotos2

#if defined(OS_WIN)
#	include "strings/string_util_win.h"
#elif defined(OS_POSIX)
#	include "strings/string_util_posix.h"
#else
#	error Define string operations appropriately for your platform
#endif

#endif // LOTOS2_STRINGS_STRING_UTIL_H
