// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license.

#ifndef LOTOS2_STRINGS_STRINGPRINTF_H
#define LOTOS2_STRINGS_STRINGPRINTF_H

#include <cstdarg>
#include <string>

#include "system/compiler_specific.h"



// Annotate a function indicating the caller must examine the return value.
// Use like:
//   int foo() WARN_UNUSED_RESULT;
// To explicitly ignore a result, see |ignore_result()| in <base/basictypes.h>.
#if defined(COMPILER_GCC)
#	define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#	define WARN_UNUSED_RESULT
#endif

// Tell the compiler a function is using a printf-style format string.
// |format_param| is the one-based index of the format string parameter;
// |dots_param| is the one-based index of the "..." parameter.
// For v*printf functions (which take a va_list), pass 0 for dots_param.
// (This is undocumented but matches what the system C headers do.)
#if defined(COMPILER_GCC)
#	define PRINTF_FORMAT(format_param, dots_param) __attribute__((format(printf, format_param, dots_param)))
#else
#	define PRINTF_FORMAT(format_param, dots_param)
#endif

namespace lotos2 {
	namespace strings {

// Return a C++ string given printf-like input.
std::string StringPrintf(const char* format, ...)
	PRINTF_FORMAT(1, 2) WARN_UNUSED_RESULT;
// Return a C++ string given vprintf-like input.
std::string StringPrintV(const char* format, va_list ap)
	PRINTF_FORMAT(1, 0) WARN_UNUSED_RESULT;

// Lower-level routine that takes a va_list and appends to a specified
// string.  All other routines are just convenience wrappers around it.
void StringAppendV(std::string* dst, const char* format, va_list ap)
	PRINTF_FORMAT(2, 0);

	}  // namespace strings
}  // namespace lotos2

#endif // LOTOS2_STRINGS_STRINGPRINTF_H
