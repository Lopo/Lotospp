cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
cmake_policy(VERSION 3.0)
message(STATUS "Running cmake version ${CMAKE_VERSION}")

project(Lotos++)

set(LOTOSPP_VERSION_MAJOR 0)
set(LOTOSPP_VERSION_MINOR 2)
set(LOTOSPP_VERSION_PATCH 2)
# Change to false for releases
set(DEVELOPMENT_BUILD true)

set(CMAKE_VERBOSE_MAKEFILE true)
if (DEFINED CMAKE_BUILD_TYPE)
	set(HAVE_CMAKE_BUILD_TYPE TRUE)
endif ()
set(CUSTOM_C_FLAGS $ENV{CFLAGS})

get_filename_component(ROOT_DIR ${CMAKE_SOURCE_DIR} REALPATH CACHE)
set(SOURCE_DIR ${ROOT_DIR}/src)
set(INCLUDE_DIR ${ROOT_DIR}/include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#-----------------------------------------------------------------------
# Find packages and other useful checks
#-----------------------------------------------------------------------

# Look for our custom modules in cmake/Modules:
set(CMAKE_MODULE_PATH "${ROOT_DIR}/cmake/Modules")

# includes
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckLibraryExists)

# Options
option(ENABLE_MYSQL "Enable use of MySQL" ON)
option(ENABLE_DOXYGEN "Build docs via Doxygen" OFF)
option(WITH_DEBUG "Enable debug things" ON)
option(ENABLE_IPV6 "Enable IPv6" ON)

set(BUILDTYPE_DOCSTRING
	"Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel"
	)

if (WITH_DEBUG OR DEVELOPMENT_BUILD)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	if (UNIX AND NOT APPLE)
		# Compiling with PIC speeds up embedded build, on PIC sensitive systems
		# Predefine it to ON, in case user chooses to build embedded.
		set(WITH_PIC ON CACHE BOOL "Compile with PIC")
	endif ()
	set(OLD_WITH_DEBUG 1 CACHE INTERNAL "" FORCE)
elseif (NOT HAVE_CMAKE_BUILD_TYPE OR OLD_WITH_DEBUG)
	if (CUSTOM_C_FLAGS)
		set(CMAKE_BUILD_TYPE "" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	else (CMAKE_BUILDTYPE MATCHES "Debug" OR NOT HAVE_CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	endif ()
	set(OLD_WITH_DEBUG 0 CACHE INTERNAL "" FORCE)
endif ()

include(cmake/CommonCompilerFlags.cmake)

# Network libraries (needed on Solaris)
check_library_exists(socket getaddrinfo "" HAVE_LIBSOCKET)
if (HAVE_LIBSOCKET)
	set(SOCKET_LIBRARIES ${SOCKET_LIBRARIES} socket)
endif ()
check_library_exists(nsl gethostbyname "" HAVE_LIBNSL)
if (HAVE_LIBNSL)
	set(SOCKET_LIBRARIES ${SOCKET_LIBRARIES} nsl)
endif ()
# Stacktrace library (needed on FreeBSD)
check_library_exists(execinfo backtrace_symbols "" HAVE_LIBEXECINFO)
if (HAVE_LIBEXECINFO)
	set(EXEC_LIBRARIES ${EXEC_LIBRARIES} execinfo)
endif ()
# Compat library (needed on FreeBSD)
check_library_exists(compat ftime "" HAVE_LIBCOMPAT)
if (HAVE_LIBCOMPAT)
	set(COMPAT_LIBRARIES ${COMPAT_LIBRARIES} compat)
endif ()

# DB
set(USE_DATABASE OFF)
if (ENABLE_MYSQL)
	find_package(MySQL)
	if (MYSQL_FOUND)
		set(USE_DATABASE TRUE)
	endif ()
endif ()

check_function_exists(fork HAVE_FORK)
check_include_file(direct.h HAVE_DIRECT_H)

# Generate config.h
configure_file(
	${ROOT_DIR}/cmake/config.h.in
	${INCLUDE_DIR}/config.h
	)
# Generate version.h
include(cmake/Version.cmake)

# Doxygen is option
if (ENABLE_DOXYGEN)
	find_package(Doxygen)
	if (DOXYGEN_DOT_FOUND)
		execute_process(COMMAND
			${DOXYGEN_DOT_EXECUTABLE} -V
#			OUTPUT_VARIABLE _DOTEXEC_VER
			ERROR_VARIABLE _DOTEXEC_VER
#			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
			)
		if (_DOTEXEC_VER)
			set (DOTEXEC_VERSION_FULL "${_DOTEXEC_VER}")
			string(REGEX
				MATCH " ([0-9]+[.0-9]+)[ \\t]"
				tmp
				"${_DOTEXEC_VER}"
				)
			set(DOTEXEC_VERSION "${CMAKE_MATCH_1}")
		endif ()
		message("Found dot program: ${DOXYGEN_DOT_EXECUTABLE} ver ${DOTEXEC_VERSION}")
		set(DOT1810PLUS "NO")
		if (${DOTEXEC_VERSION} VERSION_GREATER "1.8.10")
			set(DOT1810PLUS "YES")
		endif()
		get_filename_component(DOXYGEN_DOT_EXE "${DOXYGEN_DOT_EXECUTABLE}" DIRECTORY)
		file(TO_NATIVE_PATH "${DOXYGEN_DOT_EXE}" DOXYGEN_DOT_PATH)
	endif ()
	set (DOXY_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	message(STATUS "Generating ${DOXY_CONFIG}")
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in"
		${DOXY_CONFIG} @ONLY
		)
	add_custom_target(
		docs
		${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG}
		DEPENDS ${DOXY_CONFIG}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM
		)
endif ()

set(GENERATED_INCLUDES)
set(GENERATED_SOURCES)

include_directories(include)
set(SUBDIRS
	gsrc
	src
	)
foreach(subdir ${SUBDIRS})
	add_subdirectory(${subdir})
endforeach()

include(cmake/Macros.cmake)
message("Building with support for:")
show_end_message_yesno("MySQL" ENABLE_MYSQL)
show_end_message_yesno("Doxygen" ENABLE_DOXYGEN)
show_end_message_yesno("debug things" WITH_DEBUG)
