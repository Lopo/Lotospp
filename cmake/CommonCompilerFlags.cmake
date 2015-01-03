if(USE_CLANG)
#	SET (...)
#	....
endif(USE_CLANG)

# Pthreads is required
set(CMAKE_THREAD_PREFER_PTHREAD 1)
#from libfreenect
if(WIN32)
	set(THREADS_USE_PTHREADS_WIN32 true)
endif(WIN32)
find_package(Threads)
#from log4cc
if(CMAKE_THREAD_LIBS_INIT)
	if(CMAKE_USE_WIN32_THREADS_INIT)
		set(USE_MSTHREADS TRUE)
		message(STATUS "Threading support enabled using win32 threads")
	endif(CMAKE_USE_WIN32_THREADS_INIT)
	if(CMAKE_USE_PTHREADS_INIT)
		set(USE_PTHREADS TRUE)
		message(STATUS "Threading support enabled using pthreads")
	endif(CMAKE_USE_PTHREADS_INIT)
endif(CMAKE_THREAD_LIBS_INIT)
if(NOT CMAKE_USE_PTHREADS_INIT)
	message(FATAL_ERROR "Could not find POSIX threads")
endif(NOT CMAKE_USE_PTHREADS_INIT)
#add_definitions(-D_REENTRANT)
if(THREADS_PTHREADS_INCLUDE_DIR)
	include_directories(${THREADS_PTHREADS_INCLUDE_DIR})
endif(THREADS_PTHREADS_INCLUDE_DIR)

# Boost is required for all plugins as it is used in Lotos2 includes
add_definitions(-DBOOST_LOG_DYN_LINK)
#add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DBOOST_DATE_TIME_NO_LIB)
#add_definitions(-DBOOST_REGEX_NO_LIB)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.57.0 COMPONENTS program_options system filesystem thread date_time random REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIR})
if(WIN32)
	add_definitions(-DBOOST_ASIO_HAS_MOVE)
endif(WIN32)
if(WIN32 AND __COMPILER_GNU)
	# mingw-gcc fails to link boost::thread
	add_definitions(-DBOOST_THREAD_USE_LIB)
endif(WIN32 AND __COMPILER_GNU)

include(CheckCXXCompilerFlag)

# Add -Wall if supported by compiler
check_cxx_compiler_flag(-Wall CXX_COMPILER_WALL)
if(CXX_COMPILER_WALL)
	add_compile_options(-Wall)
endif(CXX_COMPILER_WALL)

# Add -Wextra if supported by compiler
check_cxx_compiler_flag(-Wextra CXX_COMPILER_WEXTRA)
if(CXX_COMPILER_WEXTRA)
	add_compile_options(-Wextra)
endif(CXX_COMPILER_WEXTRA)

# Add -std=c++11 if supported by compiler
check_cxx_compiler_flag(-std=c++11 CXX_COMPILER_STD_CPP11)
if(CXX_COMPILER_STD_CPP11)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	add_compile_options(-std=c++11)
endif(CXX_COMPILER_STD_CPP11)

# Add -std=c++14 if supported by compiler
#check_cxx_accepts_flag(-std=c++14 CXX_ACCEPTS_STD_CPP14)
#if(CXX_ACCEPTS_STD_CPP14)
#	add_definitions(-std=c++14)
#endif(CXX_ACCEPTS_STD_CPP14)
