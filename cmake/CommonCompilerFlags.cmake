# Make sure the compiler can compile C++11 code
if (MSVC)
	if ("${MSVC_VERSION}" LESS 1800)
		message(FATAL_ERROR "Requires Visual Studio 12 2013 or newer")
	endif ()
else (MSVC)
	find_package(CXX11 REQUIRED)
endif ()

if (WIN32)
	get_WIN32_WINNT(ver)
	add_definitions(-D_WIN32_WINNT=${ver})
endif ()

# Boost is required for all plugins as it is used in Lotos++ includes
add_definitions(-DBOOST_LOG_DYN_LINK)
#add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DBOOST_DATE_TIME_NO_LIB)
#add_definitions(-DBOOST_REGEX_NO_LIB)
if (WIN32)
	set(Boost_USE_STATIC_LIBS TRUE)
endif ()
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_MODULES program_options system filesystem thread date_time random log)
#if (WITH_TEST)
#	list(APPEND BOOST_MODULES unit_test_framework)
#endif ()
find_package(Boost 1.56.0 COMPONENTS ${BOOST_MODULES} REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIR})
if (WIN32)
	add_definitions(-DBOOST_ASIO_HAS_MOVE)
	if (__COMPILER_GNU)
		# mingw-gcc fails to link boost::thread
		add_definitions(-DBOOST_THREAD_USE_LIB)
	endif ()
endif ()

include(CheckCXXCompilerFlag)

# Add -Wall if supported by compiler
check_cxx_compiler_flag(-Wall CXX_COMPILER_WALL)
if (CXX_COMPILER_WALL)
	add_compile_options(-Wall)
endif ()

# Add -Wextra if supported by compiler
check_cxx_compiler_flag(-Wextra CXX_COMPILER_WEXTRA)
if (CXX_COMPILER_WEXTRA)
	add_compile_options(-Wextra)
endif ()

# Add -Wextra if supported by compiler
check_cxx_compiler_flag(-Wno-unused-parameter CXX_COMPILER_WNOUNUSEDPARAMETER)
if (CXX_COMPILER_WNOUNUSEDPARAMETER)
	add_compile_options(-Wno-unused-parameter)
endif ()

if (FORCE32)
	check_c_compiler_flag(-m32 C_COMPILER_M32)
	check_cxx_compiler_flag(-m32 CXX_COMPILER_M32)
	if (CXX_COMPILER_M32 AND C_COMPILER_M32)
		add_compile_options(-m32)
	endif ()
endif ()
