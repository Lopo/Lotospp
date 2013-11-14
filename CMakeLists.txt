cmake_minimum_required(VERSION 2.8)
project(Lotos2)

#set(CMAKE_VERBOSE_MAKEFILE true)

set(LOTOS2_MAJOR_VERSION 0)
set(LOTOS2_MINOR_VERSION 2)
set(LOTOS2_PATCH_VERSION 0)
set(LOTOS2_STATE_VERSION "dev")
set(LOTOS2_VERSION
	${LOTOS2_MAJOR_VERSION}.${LOTOS2_MINOR_VERSION}.${LOTOS2_PATCH_VERSION})

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
	set(DEF_INSTALL_CMAKE_DIR CMake)
else()
	set(DEF_INSTALL_CMAKE_DIR lib/CMake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
	set(var INSTALL_${p}_DIR)
	if(NOT IS_ABSOLUTE "${${var}}")
		set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
	endif()
endforeach()

# set up include-directories
include_directories(
	"${PROJECT_BINARY_DIR}"
	)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE Lotos2)


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
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/include/config.h
	)

# make uninstall (file added to top binray dir to work with build_all)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
configure_file(
	cmake/Lotos2ConfigVersion.cmake.in
	"${PROJECT_BINARY_DIR}/Lotos2ConfigVersion.cmake" @ONLY
	)

add_custom_target(
	uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)

set(SUBDIRS	src)
foreach(subdir ${SUBDIRS})
	add_subdirectory(${subdir})
endforeach()

check_include_files(direct.h HAVE_DIRECT_H)

#if(UNIX)
#	set(LP_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
#	set(LP_INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
#endif()

#set(BUILD_SHARED_LIBS ON)  # By default all libs will be built as shared libs

#-----------------------------------------------------------------------
# Define some useful variables and options
#-----------------------------------------------------------------------

#include_directories(include)

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