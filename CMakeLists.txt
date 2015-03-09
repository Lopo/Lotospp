cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
cmake_policy(VERSION 3.1)

project(Lotos2)

set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_BUILD_TYPE Debug)
if (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo
		CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE
		)
endif (NOT CMAKE_BUILD_TYPE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#-----------------------------------------------------------------------
# Find packages and other useful checks
#-----------------------------------------------------------------------

# Look for our custom modules in cmake/Modules:
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

# includes
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckLibraryExists)

# Options
option(ENABLE_MYSQL "Enable use of MySQL" OFF)
option(ENABLE_DOXYGEN "Build docs via Doxygen" OFF)
option(USE_DEBUG "Enable debug things" ON)

include(cmake/CommonCompilerFlags.cmake)

# Network libraries (needed on Solaris)
check_library_exists(socket getaddrinfo "" HAVE_LIBSOCKET)
if (HAVE_LIBSOCKET)
	set(SOCKET_LIBRARIES ${SOCKET_LIBRARIES} socket)
endif (HAVE_LIBSOCKET)
check_library_exists(nsl gethostbyname "" HAVE_LIBNSL)
if (HAVE_LIBNSL)
	set(SOCKET_LIBRARIES ${SOCKET_LIBRARIES} nsl)
endif (HAVE_LIBNSL)

# DB
set(USE_DATABASE OFF)
if (ENABLE_MYSQL)
	find_package(MySQL)
	if (MYSQL_FOUND)
		set(USE_DATABASE ON)
		add_definitions(-DENABLE_MYSQL)
	endif (MYSQL_FOUND)
endif (ENABLE_MYSQL)

check_function_exists(fork HAVE_FORK)
check_include_files(direct.h HAVE_DIRECT_H)

# Generate config.h
configure_file(
	${PROJECT_SOURCE_DIR}/cmake/config.h.in
	${PROJECT_SOURCE_DIR}/include/config.h
	)
# Generate version.h
include(cmake/Version.cmake)

# Doxygen is option
if (ENABLE_DOXYGEN)
	find_package(Doxygen)
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
		COMMENT "Generating doxygen documentation"
		VERBATIM
		)
endif (ENABLE_DOXYGEN)

set(SUBDIRS
	src
	)
foreach(subdir ${SUBDIRS})
	add_subdirectory(${subdir})
endforeach()

include_directories(include)

include(cmake/Macros.cmake)
message("Building with support for:")
show_end_message_yesno("MySQL" ENABLE_MYSQL)
show_end_message_yesno("Doxygen" ENABLE_DOXYGEN)
show_end_message_yesno("debug things" USE_DEBUG)