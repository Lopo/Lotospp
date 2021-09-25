// This file adds defines about the platform we're currently building on.
//  Operating System:
//    OS_WIN / OS_MACOSX / OS_LINUX / OS_POSIX (MACOSX or LINUX) /
//    OS_NACL (NACL_SFI or NACL_NONSFI) / OS_NACL_SFI / OS_NACL_NONSFI
//  Compiler:
//    COMPILER_MSVC / COMPILER_GCC
//  Processor:
//    ARCH_CPU_X86 / ARCH_CPU_X86_64 / ARCH_CPU_X86_FAMILY (X86 or X86_64)
//    ARCH_CPU_32_BITS / ARCH_CPU_64_BITS

#ifndef LOTOSPP_SYSTEM_BUILD_CONFIG_H
#define LOTOSPP_SYSTEM_BUILD_CONFIG_H

// A set of macros to use for platform detection
#if defined(__native_client__)
// __native_client__ must be first, so that other OS_ defines are not set
#	define OS_NACL 1
// OS_NACL comes in two sandboxing technology flavors, SFI or Non-SFI
// PNaCl toolchain defines __native_client_nonsfi__ macro in Non-SFI build mode, while it does not in SFI build mode
#	if defined(__native_client_nonsfi__)
#		define OS_NACL_NONSFI
#	else
#		define OS_NACL_SFI
#	endif
#elif defined(__APPLE__)
#	include <TargetConditionals.h>
#	define OS_MACOSX 1
#	if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#		define OS_IOS 1
#	endif // defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#elif defined(__linux) || defined(__linux__) || defined(linux)
#	define OS_LINUX 1
// include a system header to pull in features.h for glibc/uclibc macros
#	include <unistd.h>
#	if defined(__GLIBC__) && !defined(__UCLIBC__)
// we really are using glibc, not uClibc pretending to be glibc
#		define LIBC_GLIBC 1
#	endif
#elif defined(_WIN32)
#	define OS_WIN 1
#elif defined(__FreeBSD__)
#	define OS_FREEBSD 1
#elif defined(__OpenBSD__)
#	define OS_OPENBSD 1
#elif defined(__NetBSD__)
#	define OS_NETBSD 1
#elif defined(__sun)
#	define OS_SOLARIS 1
#elif defined(__QNXNTO__)
#	define OS_QNX 1
#else
#	error Please add support for your platform in build/build_config.h
#endif

// For access to standard BSD features, use OS_BSD instead of a more specific macro
#if defined(OS_FREEBSD) || defined(OS_OPENBSD) || defined(OS_NETBSD)
#	define OS_BSD 1
#endif

// For access to standard POSIXish features, use OS_POSIX instead of a more specific macro
#if defined(OS_MACOSX) || defined(OS_LINUX) || defined(OS_BSD) || defined(OS_SOLARIS) || \
	defined(OS_NACL) || defined(OS_QNX)
#	define OS_POSIX 1
#endif

// Compiler detection
#if defined(__GNUC__)
#	define COMPILER_GCC 1
#	if defined(__clang__)
#		define COMPILER_CLANG 1
#	elif defined(__MINGW32__)
#		define COMPILER_MINGW 1
#	endif
#elif defined(_MSC_VER)
#	define COMPILER_MSVC 1
#else
#	error Please add support for your compiler in build/build_config.h
#endif

// Processor architecture detection. For more info on what's defined, see:
//   http://msdn.microsoft.com/en-us/library/b0084kay.aspx
//   http://www.agner.org/optimize/calling_conventions.pdf
//  or with gcc, run: "echo | gcc -E -dM -"
#if defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
#	define ARCH_CPU_X86_FAMILY 1
#	define ARCH_CPU_X86_64 1
#	define ARCH_CPU_64_BITS 1
#	define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
#	define ARCH_CPU_X86_FAMILY 1
#	define ARCH_CPU_X86 1
#	define ARCH_CPU_32_BITS 1
#	define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__ARMEL__)
#	define ARCH_CPU_ARM_FAMILY 1
#	define ARCH_CPU_ARMEL 1
#	define ARCH_CPU_32_BITS 1
#	define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__aarch64__)
#	define ARCH_CPU_ARM_FAMILY 1
#	define ARCH_CPU_ARM64 1
#	define ARCH_CPU_64_BITS 1
#	define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__pnacl__)
#	define ARCH_CPU_32_BITS 1
#	define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__MIPSEL__)
#	if defined(__LP64__)
#		define ARCH_CPU_MIPS64_FAMILY 1
#		define ARCH_CPU_MIPS64EL 1
#		define ARCH_CPU_64_BITS 1
#		define ARCH_CPU_LITTLE_ENDIAN 1
#	else
#		define ARCH_CPU_MIPS_FAMILY 1
#		define ARCH_CPU_MIPSEL 1
#		define ARCH_CPU_32_BITS 1
#		define ARCH_CPU_LITTLE_ENDIAN 1
#	endif
#else
#	error Please add support for your architecture in build/build_config.h
#endif

#endif
