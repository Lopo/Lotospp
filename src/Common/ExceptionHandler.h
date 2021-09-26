#ifndef LOTOSPP_COMMON_EXCEPTIONHANDLER_H
#define	LOTOSPP_COMMON_EXCEPTIONHANDLER_H

#include "config.h"
#ifdef __EXCEPTION_TRACER__
#	ifdef OS_WIN
#		include <Windows.h>
#	endif


namespace LotosPP::Common {

class ExceptionHandler
{
public:
	ExceptionHandler();
	~ExceptionHandler();
	bool InstallHandler();
	bool RemoveHandler();
	static void dumpStack();
private:
#ifdef OS_WIN
#	if defined(_MSC_VER) || defined(__USE_MINIDUMP__)
	static LONG WINAPI MiniDumpExceptionHandler(struct _EXCEPTION_POINTERS* pExceptionInfo);
	static int refCounter;
#	elif __GNUC__
	struct SEHChain {
		SEHChain* prev;
		void* SEHfunction;
		};
	SEHChain chain;
#	endif
#endif

	bool isInstalled{false};
};

	}

#endif // __EXCEPTION_TRACER__
#endif
