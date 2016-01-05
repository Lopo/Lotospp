#ifndef LOTOSPP_SYSTEM_COMPILER_SPECIFIC_H
#define LOTOSPP_SYSTEM_COMPILER_SPECIFIC_H


#include "system/build_config.h"

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

#endif // LOTOSPP_SYSTEM_COMPILER_SPECIFIC_H
