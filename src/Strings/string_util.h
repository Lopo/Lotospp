#ifndef LOTOSPP_STRINGS_STRING_UTIL_H
#define LOTOSPP_STRINGS_STRING_UTIL_H

#include "Common/macros.h"
#include "System/compiler_specific.h"
#include <vector>
#include <cctype>
#include <cstdarg> // va_list


namespace LotosPP::Strings {

// C standard-library functions like "strncasecmp" and "snprintf" that aren't cross-platform are provided
// as "strings::strncasecmp", and their prototypes are listed below. These functions are then implemented as inline
// calls to the platform-specific equivalents in the platform-specific headers.

// Wrapper for vsnprintf that always null-terminates and always returns the number of characters that would be in
// an untruncated formatted string, even when truncation occurs.
int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments)
	PRINTF_FORMAT(3, 0);

	}

#if defined(OS_WIN) && !defined(COMPILER_MINGW)
#	include "string_util_win.h"
#elif defined(OS_POSIX) || defined(COMPILER_MINGW)
#	include "string_util_posix.h"
#else
#	error Define string operations appropriately for your platform
#endif

#endif
