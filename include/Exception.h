#ifndef LOTOS2_EXCEPTION_H
#define	LOTOS2_EXCEPTION_H

#include "config.h"

#if defined __EXCEPTION_TRACER__

#include "system.h"


namespace lotos2 {

class ExceptionHandler
{
public:
	ExceptionHandler();
	~ExceptionHandler();
	bool InstallHandler();
	bool RemoveHandler();
	static void dumpStack();
private:
#if defined WIN32 || defined __WINDOWS__
#if defined _MSC_VER || defined __USE_MINIDUMP__

	static long __stdcall MiniDumpExceptionHandler(struct _EXCEPTION_POINTERS *pExceptionInfo);
	static int ref_counter;

#elif __GNUC__

	struct SEHChain {
		SEHChain *prev;
		void *SEHfunction;
		};
	SEHChain chain;
#endif
#endif

	bool isInstalled=false;
};

} // namespace lotos2

#endif // __EXCEPTION_TRACER__
#endif // LOTOS2_EXCEPTION_H
