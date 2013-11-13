cmake_minimum_required(VERSION 2.8)

project(Lotos2)

#set(CMAKE_VERBOSE_MAKEFILE true)

#-----------------------------------------------------------------------
# Find packages and other useful checks
#-----------------------------------------------------------------------

# Look for our custom modules in cmake/Modules:
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(CheckFunctionExists)
include(CheckIncludeFile)
#include(CheckPrototypeDefinition)
#include(CheckLibraryExists)

# Options
#option(USE_OPENSSL "Enable secure communication channels" ON)
option(USE_IPV6 "Enable IPv6 support" ON)

include(cmake/CommonCompilerFlags.cmake)

# OpenSSL
#https://github.com/vikasnkumar/wisecracker/blob/master/deps/osdefaults.cmake
if(USE_OPENSSL)
	include(FindOpenSSL)
	if(OPENSSL_FOUND)
		include_directories(${OPENSSL_INCLUDE_DIR})
		set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${OPENSSL_INCLUDE_DIR})
		check_include_file("openssl/md5.h" HAVE_OPENSSL_MD5_H)
		if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
			set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} crypto)
		endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	else(OPENSSL_FOUND)
		message(STATUS "OpenSSL not found; secure communication channels will not be available")
		set(USE_OPENSSL OFF)
	endif(OPENSSL_FOUND)
endif(USE_OPENSSL)

# IPv6
if(USE_IPV6)
	message(STATUS "IPv6 support enabled")
endif(USE_IPV6)


# Setup installation paths
include(cmake/DefaultPaths.cmake)

#check_function_exists (vasprintf HAVE_VASPRINTF)
check_function_exists(fork HAVE_FORK)
#check_function_exists(snprintf HAVE_SNPRINTF)
#check_function_exists(vsnprintf HAVE_VSNPRINTF)

check_include_files(direct.h HAVE_DIRECT_H)

# Generate config.h
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
	${CMAKE_CURRENT_SOURCE_DIR}/include/Lotos2/config.h
	)

# make uninstall (file added to top binray dir to work with build_all)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
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
check_function_exists(fork HAVE_FORK)
check_function_exists(snprintf HAVE_SNPRINTF)
check_function_exists(vsnprintf HAVE_VSNPRINTF)

check_include_files(direct.h HAVE_DIRECT_H)

#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#if(UNIX)
#	set(LP_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
#	set(LP_INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
#endif()

#set(BUILD_SHARED_LIBS ON)  # By default all libs will be built as shared libs

# Add some common compiler flags. TODO: Shouldn't this be done checking if whatever compiler
# we use supports each flag to allow for non-gcc compilers in Linux (as above)?
#if (CMAKE_COMPILER_IS_GNUCXX)
#	add_definitions(-Wall -Wsign-compare -Wextra -Wno-unused -Wno-deprecated -DTIXML_USE_STL=1)
#	add_definitions(-Wall -Wextra)
#endif()

#-----------------------------------------------------------------------
# Define some useful variables and options
#-----------------------------------------------------------------------

#include_directories(include)

#add_subdirectory(src)

#link_directories(${LOTOS2_BINARY_DIR}/

#if (UNIX)
#	set(CMAKE_CXX_FLAGS "-g -Wall")
#endif(UNIX)

#get_cmake_property(P VARIABLES)
#foreach(VAR in ${P})
#	message(STATUS "  ${VAR}=${${VAR}}")
#endforeach()

#install(TARGETS
#	DESTINATION bin
#	)