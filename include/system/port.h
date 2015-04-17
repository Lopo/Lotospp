#ifndef LOTOS2_SYSTEM_PORT_H
#define LOTOS2_SYSTEM_PORT_H


#include <cstdarg>

#include "system/build_config.h"


// It's possible for functions that use a va_list, such as StringPrintf, to
// invalidate the data in it upon use. The fix is to make a copy of the
// structure before using it and use that copy instead. va_copy is provided
// for this purpose. MSVC does not provide va_copy, so define an
// implementation here. It is not guaranteed that assignment is a copy, so the
// StringUtil.VariableArgsFunc unit test tests this capability.
#if defined(COMPILER_GCC)
#	define GG_VA_COPY(a, b) (va_copy(a, b))
#elif defined(COMPILER_MSVC)
#	define GG_VA_COPY(a, b) (a = b)
#endif

#endif // LOTOS2_SYSTEM_PORT_H
