#include "ExceptionHandler.h"

#ifdef __EXCEPTION_TRACER__

#include "globals.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>


using LotosPP::Common::ExceptionHandler;


#ifdef OS_WIN
#	if defined(_MSC_VER) || defined(__USE_MINIDUMP__)
#		include <Dbghelp.h>
#		pragma comment(lib, "DbgHelp")

		// based on dbghelp.h
		typedef BOOL (WINAPI* MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

		int ExceptionHandler::refCounter{0};
#	elif __GNUC__
#		include <excpt.h>
#		include <tlhelp32.h>

		unsigned long max_off;
		unsigned long min_off;
		typedef std::map<unsigned long, char*> FunctionMap;
		FunctionMap functionMap{};

		EXCEPTION_DISPOSITION
		__cdecl _SEHHandler(
			struct _EXCEPTION_RECORD *ExceptionRecord,
			void* EstablisherFrame,
			struct _CONTEXT *ContextRecord,
			void* DispatcherContext
			);
		void printPointer(std::ostream* output,unsigned long p);
#	endif

#else // Unix/Linux
#	include <csignal>
#	ifndef OS_OPENBSD
#		include <execinfo.h>
#		include <sys/ucontext.h>
#	endif

#	include <sys/time.h>
#	include <sys/resource.h> // POSIX.1-2001

	void _SigHandler(int signum, siginfo_t* info, void* secret);
#endif

#ifndef COMPILER_STRING
#	ifdef __GNUC__
#		define COMPILER_STRING "gcc " __VERSION__
#	else
#		define COMPILER_STRING ""
#	endif
#endif

#define COMPILATION_DATE __DATE__ " " __TIME__

ExceptionHandler::ExceptionHandler()
{
}

ExceptionHandler::~ExceptionHandler()
{
	if (isInstalled) {
		RemoveHandler();
		}
}

bool ExceptionHandler::InstallHandler()
{
#ifdef OS_WIN
#	if defined(_MSC_VER) || defined(__USE_MINIDUMP__)
	++refCounter;
	if (refCounter==1) {
		::SetUnhandledExceptionFilter(ExceptionHandler::MiniDumpExceptionHandler);
		}
#	elif __GNUC__
	if (isInstalled==true) {
		return false;
		}

	SEHChain* prevSEH;
	__asm__("movl %%fs:0,%%eax;movl %%eax,%0;":"=r"(prevSEH)::"%eax");
	chain.prev=prevSEH;
	chain.SEHfunction=(void*)&_SEHHandler;
	__asm__("movl %0,%%eax;movl %%eax,%%fs:0;": : "g" (&chain):"%eax");
#	endif
#else // Unix/Linux
	struct sigaction sa;
	sa.sa_sigaction=&_SigHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags= SA_RESTART|SA_SIGINFO;

	sigaction(SIGILL, &sa, NULL); // illegal instruction
	sigaction(SIGSEGV, &sa, NULL); // segmentation fault
	sigaction(SIGFPE, &sa, NULL); // floating-point exception
#endif

	isInstalled=true;
	return true;
}

bool ExceptionHandler::RemoveHandler()
{
	if (!isInstalled) {
		return false;
		}

#ifdef OS_WIN
#	if defined(_MSC_VER) || defined(__USE_MINIDUMP__)
		--refCounter;
		if (refCounter==0) {
			::SetUnhandledExceptionFilter(NULL);
			}
#	elif __GNUC__
		/*
		mov eax,[chain.prev]
		mov fs:[0],eax
		*/
		__asm__("movl %0,%%eax;movl %%eax,%%fs:0;"::"r"(chain.prev):"%eax");
#	endif
#else // Unix/Linux
	signal(SIGILL, SIG_DFL); // illegal instruction
	signal(SIGSEGV, SIG_DFL); // segmentation fault
	signal(SIGFPE, SIG_DFL); // floating-point exception
#endif

	isInstalled=false;
	return true;
}

#ifdef OS_WIN
#	if defined(_MSC_VER) || defined(__USE_MINIDUMP__)

LONG WINAPI ExceptionHandler::MiniDumpExceptionHandler(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
	HMODULE hDll=NULL;
	char szAppPath[_MAX_PATH];
	std::string strAppDirectory;

	GetModuleFileName(NULL, szAppPath, _MAX_PATH);
	strAppDirectory=std::string(szAppPath).substr(0, strAppDirectory.rfind("\\"));
	if (strAppDirectory.rfind('\\')!=strAppDirectory.size()) {
		strAppDirectory+='\\';
		}

	std::string strFileNameDbgHelp=strAppDirectory+"DBGHELP.DLL";
	hDll=::LoadLibrary(strFileNameDbgHelp.c_str());

	if (!hDll) {
		// load any version we can
		hDll=::LoadLibrary("DBGHELP.DLL");
		}

	if (!hDll) {
		std::cout << "Could not generate report - DBGHELP.DLL could not be found." << std::endl;
		return EXCEPTION_CONTINUE_SEARCH;
		}

	MINIDUMPWRITEDUMP pDump=(MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
	if (!pDump) {
		std::cout << "Could not generate report - DBGHELP.DLL is to old." << std::endl;
		return EXCEPTION_CONTINUE_SEARCH;
		}

	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);

	char dumpfile[250]={'\0'};
	sprintf(dumpfile, "%04d-%02d-%02d_%02d%02d%02d.dmp",
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

	std::string strFileNameDump=strAppDirectory+dumpfile;

	HANDLE hFile=::CreateFile(strFileNameDump.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile==INVALID_HANDLE_VALUE) {
		std::cout << "Could not create memory dump file." << std::endl;
		return EXCEPTION_EXECUTE_HANDLER;
		}

	_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

	ExInfo.ThreadId=::GetCurrentThreadId();
	ExInfo.ExceptionPointers=pExceptionInfo;
	ExInfo.ClientPointers=NULL;

	std::cout << "Generating minidump file... " << dumpfile << std::endl;

	if (!pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL)) {
		std::cout << "Could not dump memory to file." << std::endl;
		::CloseHandle(hFile);
		return EXCEPTION_CONTINUE_SEARCH;
		}

	::CloseHandle(hFile);
	return EXCEPTION_EXECUTE_HANDLER;
}

#	elif __GNUC__

char* getFunctionName(unsigned long addr, unsigned long& start)
{
	if (addr>=min_off && addr<=max_off) {
		for (FunctionMap::iterator functions=functionMap.begin(); functions!=functionMap.end(); ++functions) {
			if (functions->first>addr && functions!=functionMap.begin()) {
				functions--;
				start=functions->first;
				return functions->second;
				break;
				}
			}
		}
	return NULL;
}

EXCEPTION_DISPOSITION
__cdecl _SEHHandler(
	struct _EXCEPTION_RECORD* ExceptionRecord,
	void* EstablisherFrame,
	struct _CONTEXT* ContextRecord,
	void* DispatcherContext
	)
{
	//
	unsigned long* esp;
	unsigned long* next_ret;
	unsigned long stack_val;
	unsigned long* stacklimit;
	unsigned long* stackstart;
	unsigned long nparameters{0};
	unsigned long file, foundRetAddress{0};
	_MEMORY_BASIC_INFORMATION mbi;

	std::ostream* outdriver;
	std::cout << "Error: generating report file..." << std::endl;
	std::ofstream output("report.txt", std::ios_base::app);
	if (output.fail()) {
		outdriver=&std::cout;
		file=false;
		}
	else {
		file=true;
		outdriver=&output;
		}

	time_t rawtime;
	time(&rawtime);
	*outdriver << "*****************************************************" << std::endl;
	*outdriver << "Error report - " << std::ctime(&rawtime) << std::endl;
	*outdriver << "Compiler info - " << COMPILER_STRING << std::endl;
	*outdriver << "Compilation Date - " << COMPILATION_DATE << std::endl << std::endl;

	//system and process info
	//- global memory information
	MEMORYSTATUSEX mstate;
	mstate.dwLength=sizeof(mstate);
	if (GlobalMemoryStatusEx(&mstate)) {
		*outdriver << "Memory load: " << mstate.dwMemoryLoad << std::endl
			<< "Total phys: " << mstate.ullTotalPhys/1024 << " K available phys: "
			<< mstate.ullAvailPhys/1024 << " K" << std::endl;
		}
	else {
		*outdriver << "Memory load: Error" << std::endl;
		}
	//-process info
	FILETIME FTcreation, FTexit, FTkernel, FTuser;
	SYSTEMTIME systemtime;
	GetProcessTimes(GetCurrentProcess(), &FTcreation, &FTexit, &FTkernel, &FTuser);
	// creation time
	FileTimeToSystemTime(&FTcreation, &systemtime);
	*outdriver << "Start time: " << systemtime.wDay << "-" << systemtime.wMonth << "-" << systemtime.wYear << "  "
		<< systemtime.wHour << ":" << systemtime.wMinute << ":" << systemtime.wSecond << std::endl;
	// kernel time
	unsigned long miliseconds;
	miliseconds=FTkernel.dwHighDateTime*429497+FTkernel.dwLowDateTime/10000;
	*outdriver << "Kernel time: " << miliseconds/3600000;
	miliseconds=miliseconds-(miliseconds/3600000)*3600000;
	*outdriver << ":" << miliseconds/60000;
	miliseconds=miliseconds-(miliseconds/60000)*60000;
	*outdriver << ":" << miliseconds/1000;
	miliseconds=miliseconds-(miliseconds/1000)*1000;
	*outdriver << "." << miliseconds << std::endl;
	// user time
	miliseconds=FTuser.dwHighDateTime*429497+FTuser.dwLowDateTime/10000;
	*outdriver << "User time: " << miliseconds/3600000;
	miliseconds=miliseconds-(miliseconds/3600000)*3600000;
	*outdriver << ":" << miliseconds/60000;
	miliseconds=miliseconds-(miliseconds/60000)*60000;
	*outdriver << ":" << miliseconds/1000;
	miliseconds=miliseconds-(miliseconds/1000)*1000;
	*outdriver << "." << miliseconds << std::endl;

	// n threads
	PROCESSENTRY32 uProcess;
	BOOL r;
	if (HANDLE lSnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); lSnapShot!=0) {
		uProcess.dwSize=sizeof(uProcess);
		r=Process32First(lSnapShot, &uProcess);
		while (r) {
			if (uProcess.th32ProcessID==GetCurrentProcessId()) {
				*outdriver << "Threads: " << uProcess.cntThreads << std::endl;
				break;
				}
			r=Process32Next(lSnapShot, &uProcess);
			}
		CloseHandle(lSnapShot);
		}

	*outdriver << std::endl;
	//exception header type and eip
	outdriver->flags(std::ios::hex | std::ios::showbase);
	*outdriver << "Exception: " << (unsigned long)ExceptionRecord->ExceptionCode
		<< " at eip = " << (unsigned long)ExceptionRecord->ExceptionAddress;
	FunctionMap::iterator functions;
	unsigned long functionAddr;
	if (char* functionName=getFunctionName((unsigned long)ExceptionRecord->ExceptionAddress, functionAddr); functionName) {
		*outdriver << "(" << functionName << " - " << functionAddr <<")";
		}
	*outdriver << std::endl ;

	//registers
	*outdriver << "eax = "; printPointer(outdriver, ContextRecord->Eax); *outdriver << std::endl;
	*outdriver << "ebx = "; printPointer(outdriver, ContextRecord->Ebx); *outdriver << std::endl;
	*outdriver << "ecx = "; printPointer(outdriver, ContextRecord->Ecx); *outdriver << std::endl;
	*outdriver << "edx = "; printPointer(outdriver, ContextRecord->Edx); *outdriver << std::endl;
	*outdriver << "esi = "; printPointer(outdriver, ContextRecord->Esi); *outdriver << std::endl;
	*outdriver << "edi = "; printPointer(outdriver, ContextRecord->Edi); *outdriver << std::endl;
	*outdriver << "ebp = "; printPointer(outdriver, ContextRecord->Ebp); *outdriver << std::endl;
	*outdriver << "esp = "; printPointer(outdriver, ContextRecord->Esp); *outdriver << std::endl;
	*outdriver << "efl = " << ContextRecord->EFlags << std::endl;
	*outdriver << std::endl;

	//stack dump
	esp=(unsigned long*)(ContextRecord->Esp);
	VirtualQuery(esp, &mbi, sizeof(mbi));
	stacklimit=(unsigned long*)((unsigned long)(mbi.BaseAddress)+mbi.RegionSize);

	*outdriver << "---Stack Trace---" << std::endl;
	*outdriver << "From: " << (unsigned long)esp << " to: " << (unsigned long)stacklimit << std::endl;

	stackstart=esp;
	next_ret=(unsigned long*)(ContextRecord->Ebp);
	unsigned long frame_param_counter;
	frame_param_counter=0;
	while (esp<stacklimit) {
		stack_val=*esp;
		if (foundRetAddress) {
			nparameters++;
			}

		if (esp-stackstart<20 || nparameters<10 || std::abs(esp-next_ret)<10 || frame_param_counter<8) {
			*outdriver << (unsigned long)esp << " | ";
			printPointer(outdriver,stack_val);
			if (esp==next_ret) {
				*outdriver << " \\\\\\\\\\\\ stack frame //////";
				}
			else if (esp-next_ret==1) {
				*outdriver << " <-- ret" ;
				}
			else if (esp-next_ret==2) {
				next_ret=(unsigned long*)*(esp-2);
				frame_param_counter=0;
				}
			frame_param_counter++;
			*outdriver<< std::endl;
			}
		if (stack_val>=min_off && stack_val<=max_off) {
			foundRetAddress++;
			//
			unsigned long functionAddr;
			char* functionName=getFunctionName(stack_val, functionAddr);
			output << (unsigned long)esp << "  " << functionName << "(" << functionAddr << ")" << std::endl;
			}
		esp++;
		}
	*outdriver << "*****************************************************" << std::endl;
	if (file) {
		((std::ofstream*)outdriver)->close();
		}
	std::cout << "Error report generated. Killing server." <<std::endl;
	exit(EXIT_FAILURE); //force exit
	return ExceptionContinueSearch;
}

void printPointer(std::ostream* output,unsigned long p)
{
	*output << p;
	if (IsBadReadPtr((void*)p,4)==0) {
		*output << " -> " << *(unsigned long*)p;
		}
}

void ExceptionHandler::dumpStack()
{
	unsigned long* esp;
	unsigned long* next_ret;
	unsigned long stack_val;
	unsigned long* stacklimit;
	unsigned long* stackstart;
	unsigned long nparameters=0;
	unsigned long foundRetAddress=0;
	_MEMORY_BASIC_INFORMATION mbi;

	std::cout << "Error: generating report file..." << std::endl;
	std::ofstream output("report.txt", std::ios_base::app);
	output.flags(std::ios::hex | std::ios::showbase);
	time_t rawtime;
	time(&rawtime);
	output << "*****************************************************" << std::endl;
	output << "Stack dump - " << std::ctime(&rawtime) << std::endl;
	output << "Compiler info - " << COMPILER_STRING << std::endl;
	output << "Compilation Date - " << COMPILATION_DATE << std::endl << std::endl;

	__asm__ ("movl %%esp, %0;":"=r"(esp)::);

	VirtualQuery(esp, &mbi, sizeof(mbi));
	stacklimit=(unsigned long*)((unsigned long)(mbi.BaseAddress)+mbi.RegionSize);

	output << "---Stack Trace---" << std::endl;
	output << "From: " << (unsigned long)esp << " to: " << (unsigned long)stacklimit << std::endl;

	stackstart=esp;
	__asm__ ("movl %%ebp, %0;":"=r"(next_ret)::);

	unsigned long frame_param_counter{0};
	while (esp<stacklimit) {
		stack_val=*esp;
		if (foundRetAddress) {
			nparameters++;
			}

		if (esp-stackstart<20 || nparameters<10 || std::abs(esp-next_ret)<10 || frame_param_counter<8) {
			output << (unsigned long)esp << " | ";
			printPointer(&output, stack_val);
			if (esp==next_ret) {
				output << " \\\\\\\\\\\\ stack frame //////";
				}
			else if (esp-next_ret==1) {
				output << " <-- ret" ;
				}
			else if (esp-next_ret==2) {
				next_ret=(unsigned long*)*(esp-2);
				frame_param_counter=0;
				}
			frame_param_counter++;
			output << std::endl;
			}
		if (stack_val>=min_off && stack_val<=max_off) {
			foundRetAddress++;
			unsigned long functionAddr;
			char* functionName=getFunctionName(stack_val, functionAddr);
			output << (unsigned long)esp << "  " << functionName << "(" << functionAddr << ")" << std::endl;
			}
		esp++;
		}
	output << "*****************************************************" << std::endl;
	output.close();
}
#	endif
#else // Unix/Linux
#define BACKTRACE_DEPTH 128
void _SigHandler(int signum, siginfo_t *info, void* void_context)
{
	bool file{false};
	ucontext_t context=*(ucontext_t*)void_context;

	std::ostream* outdriver;
	std::cout << "Error: generating report file..." << std::endl;
	std::ofstream output("report.txt", std::ios_base::app);
	if (output.fail()) {
		outdriver=&std::cout;
		file=false;
		}
	else {
		file=true;
		outdriver=&output;
		}

	time_t rawtime;
	time(&rawtime);
	*outdriver << "*****************************************************" << std::endl;
	*outdriver << "Error report - " << std::ctime(&rawtime) << std::endl;
	*outdriver << "Compiler info - " << COMPILER_STRING << std::endl;
	*outdriver << "Compilation Date - " << COMPILATION_DATE << std::endl << std::endl;

	if (rusage resources; getrusage(RUSAGE_SELF, &resources)!=-1) {
		//- global memory information
		rlimit resourcelimit;
#	ifndef OS_OPENBSD
		if (getrlimit(RLIMIT_AS, &resourcelimit)!=-1) {
#	else
		if (getrlimit(RLIMIT_DATA, &resourcelimit)!=-1) {
#	endif
			// note: This is not POSIX standard, but it is available in Unix System V release 4, Linux, and 4.3 BSD
			long memusage=resources.ru_ixrss+resources.ru_idrss+resources.ru_isrss;
			long memtotal=resourcelimit.rlim_max;
			long memavail=memtotal-memusage;
			long memload=long(float(memusage/memtotal)*100.f);
			*outdriver << "Memory load: " << memload << "K " << std::endl
				<< "Total memory: " << memtotal << "K " << "available: " << memavail << "K" << std::endl;
			}
		//-process info
		// creation time
		time_t bootTime=LotosPP::options.get<time_t>("runtime.bootTime");
		tm* ts=localtime(&bootTime);
		char date_buff[80];
		strftime(date_buff, 80, "%d-%m-%Y %H:%M:%S", ts);
		// kernel time
		*outdriver << "Kernel time: " << (resources.ru_stime.tv_sec/3600)
			<< ":" << ((resources.ru_stime.tv_sec%3600)/60)
			<< ":" << ((resources.ru_stime.tv_sec%3600)%60)
			<< "." << (resources.ru_stime.tv_usec/1000)
			<< std::endl;
		// user time
		*outdriver << "User time: " << (resources.ru_utime.tv_sec/3600)
			<< ":" << ((resources.ru_utime.tv_sec%3600)/60)
			<< ":" << ((resources.ru_utime.tv_sec%3600)%60)
			<< "." << (resources.ru_utime.tv_usec/1000)
			<< std::endl;
		}
	// TODO: Process thread count (is it really needed anymore?)
	*outdriver << std::endl;

	outdriver->flags(std::ios::hex | std::ios::showbase);
	*outdriver << "Signal: " << strsignal(signum) << " (" << signum << ")"
		// this is (void*), but using %p would print "(null)" even for ptrs which are not exactly 0, but, say, 0x123
		<< " Address: " << std::hex << std::showbase << (long)info->si_addr
		<< std::endl;
#	ifdef OS_LINUX
	greg_t esp{0};
	{
#		if __WORDSIZE == 32
		*outdriver << " at eip = " << context.uc_mcontext.gregs[REG_EIP] << std::endl;
		*outdriver << "eax = " << context.uc_mcontext.gregs[REG_EAX] << std::endl;
		*outdriver << "ebx = " << context.uc_mcontext.gregs[REG_EBX] << std::endl;
		*outdriver << "ecx = " << context.uc_mcontext.gregs[REG_ECX] << std::endl;
		*outdriver << "edx = " << context.uc_mcontext.gregs[REG_EDX] << std::endl;
		*outdriver << "esi = " << context.uc_mcontext.gregs[REG_ESI] << std::endl;
		*outdriver << "edi = " << context.uc_mcontext.gregs[REG_EDI] << std::endl;
		*outdriver << "ebp = " << context.uc_mcontext.gregs[REG_EBP] << std::endl;
		*outdriver << "esp = " << context.uc_mcontext.gregs[REG_ESP] << std::endl;
		*outdriver << "efl = " << context.uc_mcontext.gregs[REG_EFL] << std::endl;
		esp=context.uc_mcontext.gregs[REG_ESP];
#		else // 64-bit
		*outdriver << " at rip = " << context.uc_mcontext.gregs[REG_RIP] << std::endl;
		*outdriver << "rax = " << context.uc_mcontext.gregs[REG_RAX] << std::endl;
		*outdriver << "rbx = " << context.uc_mcontext.gregs[REG_RBX] << std::endl;
		*outdriver << "rcx = " << context.uc_mcontext.gregs[REG_RCX] << std::endl;
		*outdriver << "rdx = " << context.uc_mcontext.gregs[REG_RDX] << std::endl;
		*outdriver << "rsi = " << context.uc_mcontext.gregs[REG_RSI] << std::endl;
		*outdriver << "rdi = " << context.uc_mcontext.gregs[REG_RDI] << std::endl;
		*outdriver << "rbp = " << context.uc_mcontext.gregs[REG_RBP] << std::endl;
		*outdriver << "rsp = " << context.uc_mcontext.gregs[REG_RSP] << std::endl;
		*outdriver << "efl = " << context.uc_mcontext.gregs[REG_EFL] << std::endl;
		esp=context.uc_mcontext.gregs[REG_RSP];
#		endif
	}
#	elif defined(OS_OPENBSD)
	*outdriver << " at rip " << context.sc_rip << std::endl;
	*outdriver << "rax = " << context.sc_rax << std::endl;
	*outdriver << "rbx = " << context.sc_rbx << std::endl;
	*outdriver << "rcx = " << context.sc_rcx << std::endl;
	*outdriver << "rdx = " << context.sc_rdx << std::endl;
	*outdriver << "rsi = " << context.sc_rsi << std::endl;
	*outdriver << "rdi = " << context.sc_rdi << std::endl;
	*outdriver << "rbp = " << context.sc_rbp << std::endl;
	*outdriver << "rsp = " << context.sc_rsp << std::endl;
#	elif defined(OS_FREEBSD)
#		ifdef __i386__
	*outdriver << " at eip " << context.uc_mcontext.mc_eip << std::endl;
#		elif defined(__amd64__)
	*outdriver << " at rip " << context.uc_mcontext.mc_rip << std::endl;
#		endif
#	elif defined(OS_NETBSD)
	*outdriver << " at rip = " << context.uc_mcontext.__gregs[_REG_RIP] << std::endl;
	*outdriver << "rax = " << context.uc_mcontext.__gregs[_REG_RAX] << std::endl;
	*outdriver << "rbx = " << context.uc_mcontext.__gregs[_REG_RBX] << std::endl;
	*outdriver << "rcx = " << context.uc_mcontext.__gregs[_REG_RCX] << std::endl;
	*outdriver << "rdx = " << context.uc_mcontext.__gregs[_REG_RDX] << std::endl;
	*outdriver << "rsi = " << context.uc_mcontext.__gregs[_REG_RSI] << std::endl;
	*outdriver << "rdi = " << context.uc_mcontext.__gregs[_REG_RDI] << std::endl;
	*outdriver << "rbp = " << context.uc_mcontext.__gregs[_REG_RBP] << std::endl;
	*outdriver << "rsp = " << context.uc_mcontext.__gregs[_REG_RSP] << std::endl;
#	elif defined(OS_MACOSX)
#		ifdef __LP64__
	*outdriver << " at rip " << context.uc_mcontext->__ss.__rip << std::endl;
	*outdriver << "rax = " << context.uc_mcontext->__ss.__rax << std::endl;
	*outdriver << "rbx = " << context.uc_mcontext->__ss.__rbx << std::endl;
	*outdriver << "rcx = " << context.uc_mcontext->__ss.__rcx << std::endl;
	*outdriver << "rdx = " << context.uc_mcontext->__ss.__rdx << std::endl;
	*outdriver << "rsi = " << context.uc_mcontext->__ss.__rsi << std::endl;
	*outdriver << "rdi = " << context.uc_mcontext->__ss.__rdi << std::endl;
	*outdriver << "rbp = " << context.uc_mcontext->__ss.__rbp << std::endl;
	*outdriver << "rsp = " << context.uc_mcontext->__ss.__rsp << std::endl;
#		else
	*outdriver << " at eip " << context.uc_mcontext->__ss.__eip << std::endl;
	*outdriver << "eax = " << context.uc_mcontext->__ss.__eax << std::endl;
	*outdriver << "ebx = " << context.uc_mcontext->__ss.__ebx << std::endl;
	*outdriver << "ecx = " << context.uc_mcontext->__ss.__ecx << std::endl;
	*outdriver << "edx = " << context.uc_mcontext->__ss.__edx << std::endl;
	*outdriver << "esi = " << context.uc_mcontext->__ss.__esi << std::endl;
	*outdriver << "edi = " << context.uc_mcontext->__ss.__edi << std::endl;
	*outdriver << "ebp = " << context.uc_mcontext->__ss.__ebp << std::endl;
	*outdriver << "esp = " << context.uc_mcontext->__ss.__esp << std::endl;
#		endif
#	endif
	outdriver->flush();
	*outdriver << std::endl;

#	ifndef OS_OPENBSD
	// stack backtrace
	int addrs;
	void* buffer[BACKTRACE_DEPTH];
	addrs=backtrace(buffer, BACKTRACE_DEPTH);
	char** symbols=backtrace_symbols(buffer, addrs);
	if (symbols!=NULL && addrs!=0) {
		*outdriver << "---Stack Trace---" << std::endl;
#		ifdef OS_LINUX
		if (esp!=0) {
			*outdriver << "From: " << (unsigned long)esp << " to: " << (unsigned long)(esp+addrs) << std::endl;
			}
#		endif
		for (int i=0; i!=addrs; ++i) {
			*outdriver << symbols[i] << std::endl;
			}
		}
#	endif
	outdriver->flush();

	if (file) {
		((std::ofstream*)outdriver)->close();
		}

	_exit(EXIT_FAILURE);
}

void ExceptionHandler::dumpStack()
{
	return;
}
#endif

#endif
