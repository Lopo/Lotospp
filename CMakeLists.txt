cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
cmake_policy(VERSION 3.1)

project(Lotos2)

set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#-----------------------------------------------------------------------
# Find packages and other useful checks
#-----------------------------------------------------------------------

# Look for our custom modules in cmake/Modules:
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
message(STATUS "modpath: ${CMAKE_MODULE_PATH}")

# Make sure the compiler can compile C++11 code
include(FindCXX11)

include(CheckFunctionExists)
include(CheckIncludeFile)
#include(CheckPrototypeDefinition)
include(CheckLibraryExists)

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

# Options
#option(USE_OPENSSL "Enable secure communication channels" ON)
option(USE_IPV6 "Enable IPv6 support" ON)
option(USE_MYSQL "Enable use of MySQL" ON)
option(USE_CLANG "Build application with clang" OFF) # OFF is the default
option(USE_DOXYGEN "Build docs via doxygen" OFF)
option(USE_PUEBLO "Enable Pueblo support" OFF)
option(USE_DEBUG "Enable debug things" ON)

include(cmake/CommonCompilerFlags.cmake)

# Network libraries (needed on Solaris)
check_library_exists(socket getaddrinfo "" HAVE_LIBSOCKET)
if (HAVE_LIBSOCKET)
	set(SOCKET_LIBRARIES ${SOCKET_LIBRARIES} socket)
endif (HAVE_LIBSOCKET)
check_library_exists(nsl gethostbyname "" HAVE_LIBNSL)
	set(SOCKET_LIBRARIES ${SOCKET_LIBRARIES} nsl)
if (HAVE_LIBNSL)
endif (HAVE_LIBNSL)

# OpenSSL
#https://github.com/vikasnkumar/wisecracker/blob/master/deps/osdefaults.cmake
if (USE_OPENSSL)
	include(FindOpenSSL)
	if (OPENSSL_FOUND)
		include_directories(${OPENSSL_INCLUDE_DIR})
		set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${OPENSSL_INCLUDE_DIR})
		check_include_file("openssl/md5.h" HAVE_OPENSSL_MD5_H)
#		if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#			set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} crypto) # because libcrypto is not found automatically
#		endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	else(OPENSSL_FOUND)
		message(STATUS "OpenSSL not found; secure communication channels will not be available")
		set(USE_OPENSSL OFF)
	endif (OPENSSL_FOUND)
endif (USE_OPENSSL)

# IPv6
if (USE_IPV6)
	message(STATUS "IPv6 support enabled")
endif (USE_IPV6)

# DB
set(USE_DATABASE OFF)

if (USE_MYSQL)
	include(FindMySQL)
	find_package(MySQL)
	if (MYSQL_FOUND)
		set(USE_DATABASE ON)
		add_definitions(-DUSE_MYSQL)
	endif (MYSQL_FOUND)
endif (USE_MYSQL)

# Pueblo
if (USE_PUEBLO)
	message(STATUS "Pueblo support enabled")
endif (USE_PUEBLO)

#check_function_exists (vasprintf HAVE_VASPRINTF)
check_function_exists(fork HAVE_FORK)
#check_function_exists(snprintf HAVE_SNPRINTF)
#check_function_exists(vsnprintf HAVE_VSNPRINTF)

check_include_files(direct.h HAVE_DIRECT_H)


# Setup installation paths
include(cmake/DefaultPaths.cmake)

# Generate config.h
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/include/config.h
	)

# Generate version.h
include(cmake/Version.cmake)

# Doxygen is option
find_package(Doxygen)
if (USE_DOXYGEN)
	if (NOT DOXYGEN_DOT_FOUND)
		message(SEND_ERROR "Need the dot program to build documentation")
	endif (NOT DOXYGEN_DOT_FOUND)
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
		COMMENT "Generating doxygen documentation" VERBATIM
		)
endif (USE_DOXYGEN)

# make uninstall (file added to top binray dir to work with build_all)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	@ONLY
	)

add_custom_target(
	uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)

set(SUBDIRS
	cmake
	include
	src
	)
foreach(subdir ${SUBDIRS})
	add_subdirectory(${subdir})
endforeach()

#check_function_exists (vasprintf HAVE_VASPRINTF)
#check_function_exists(fork HAVE_FORK)
#check_function_exists(snprintf HAVE_SNPRINTF)
#check_function_exists(vsnprintf HAVE_VSNPRINTF)

#check_include_files(direct.h HAVE_DIRECT_H)

#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#if (UNIX)
#	set(LP_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
#	set(LP_INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
#endif (UNIX)

#set(BUILD_SHARED_LIBS ON)  # By default all libs will be built as shared libs

# Add some common compiler flags. TODO: Shouldn't this be done checking if whatever compiler
# we use supports each flag to allow for non-gcc compilers in Linux (as above)?
#if (CMAKE_COMPILER_IS_GNUCXX)
#	add_definitions(-Wall -Wsign-compare -Wextra -Wno-unused -Wno-deprecated -DTIXML_USE_STL=1)
#	add_definitions(-Wall -Wextra)
#endif (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------
# Define some useful variables and options
#-----------------------------------------------------------------------

include_directories(include)

#add_subdirectory(src)

#link_directories(${LOTOS2_BINARY_DIR}/

#if (UNIX)
#	set(CMAKE_CXX_FLAGS "-g -Wall")
#endif (UNIX)

#get_cmake_property(P VARIABLES)
#foreach(VAR in ${P})
#	message(STATUS "  ${VAR}=${${VAR}}")
#endforeach()

#install(TARGETS
#	DESTINATION bin
#	)
