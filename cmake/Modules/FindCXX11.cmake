if (__FIND_CXX11_CMAKE__)
	return ()
endif ()
set(__FIND_CXX11_CMAKE__ TRUE)

# Visual Studio 2008 (vs9) doesn't seem to support C++11 directly (only as TR1)
if (MSVC AND MSVC_VERSION GREATER 1500)
	set(CXX11_FOUND 1)
	# Visual Studio 2010 (vs10) doesn't support C++11 STL.
	if (MSVC_VERSION GREATER 1600)
		set(CXX11_STL_FOUND 1)
	endif ()
	return ()
endif ()

include(CheckCXXCompilerFlag)
enable_language(CXX)

check_cxx_compiler_flag("-std=c++11" COMPILER_KNOWS_CXX11)
if (COMPILER_KNOWS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

	# Tested on Mac OS X 10.8.2 with XCode 4.6 Command Line Tools
	# Clang requires this to find the correct c++11 headers
	check_cxx_compiler_flag("-stdlib=libc++" COMPILER_KNOWS_STDLIB)
	if (COMPILER_KNOWS_STDLIB)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	endif ()
else ()
	check_cxx_compiler_flag("-std=c++0x" COMPILER_KNOWS_CXX0X)
	if (COMPILER_KNOWS_CXX0X)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else ()
		message(FATAL_ERROR "Your C++ compiler does not support C++11.")
	endif ()
endif ()
