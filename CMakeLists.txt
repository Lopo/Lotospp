cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
#test art: figlet -f ogre Lotos++ #| sed -e 's/\\/\\\\/g'
message("
   __       _
  / /  ___ | |_ ___  ___   _     _
 / /  / _ \\| __/ _ \\/ __|_| |_ _| |_
/ /__| (_) | || (_) \\__ \\_   _|_   _|
\\____/\\___/ \\__\\___/|___/ |_|   |_|
")

find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
	# Update submodules as needed
	option(GIT_SUBMODULE "Check submodules during build" ON)
	if (GIT_SUBMODULE)
		message(STATUS "GIT Submodule update")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if (NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "${GIT_EXECUTABLE} submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif ()
	endif ()
endif ()

option(HUNTER_ENABLED "Enable Hunter package manager" YES)
set(HUNTER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.hunter CACHE PATH "") # use own local dir instead of global (for debug)
option(HUNTER_STATUS_DEBUG ON)

include(cmake/cable/bootstrap.cmake)
include(CableBuildInfo)
include(CableBuildType)
include(CableToolchains)
include(HunterGate)

cable_configure_toolchain(DEFAULT cxx11)

set(HUNTER_CONFIGURATION_TYPES Release CACHE STRING "Build type of Hunter packages")
include(ProcessorCount)
ProcessorCount(_count)
set(HUNTER_JOBS_NUMBER ${_count} CACHE STRING "Number of parallel builds used by Hunter")
HunterGate(
	URL "https://github.com/cpp-pm/hunter/archive/v0.23.314.tar.gz"
	SHA1 "95c47c92f68edb091b5d6d18924baabe02a6962a"
	LOCAL # enable cmake/Hunter/config.cmake
)
cable_set_build_type(DEFAULT Release CONFIGURATION_TYPES Release RelWithDebInfo Debug)

# Change to false for releases
set(DEVELOPMENT_BUILD TRUE)

string(FIND ${CMAKE_BUILD_TYPE} "Deb" IsDebV)
if (NOT IsDebV EQUAL -1)
	set(IsDeb TRUE)
else ()
	set(IsDeb FALSE)
endif ()

#set(CMAKE_VERBOSE_MAKEFILE ${IsDeb})
set(CMAKE_VERBOSE_MAKEFILE ON CACHE INTERNAL "")

project(Lotos++ VERSION 0.2.3)

get_filename_component(ROOT_DIR ${CMAKE_SOURCE_DIR} REALPATH CACHE)

# CMake policies
message(STATUS "setting CMake policies")
foreach (pol
		CMP0015 # link_directories() treats paths relative to the source dir
		CMP0054 # Only interpret if() arguments as variables or keywords when unquoted
		CMP0067 # Honor language standard in try_compile() source-file signature
		)
	if (POLICY ${pol})
		cmake_policy(SET ${pol} NEW)
	endif ()
endforeach ()

# Options
option(ENABLE_MYSQL "Enable use of MySQL" ON)
option(ENABLE_DOXYGEN "Build docs via Doxygen" ON)
option(WITH_DEBUG "Enable debug things" ON)
option(ENABLE_IPV6 "Enable IPv6" ON)
option(ENABLE_STRIP "Strip all symbols from executables" ON)
option(ENABLE_MULTIBUILD "Compile on all CPU cores simltaneously in MSVC" ON)
option(FORCE32 "Force 32-bit build. It will add `-m32` to compiler flags" OFF)

if (DEFINED CMAKE_BUILD_TYPE AND NOT ${CMAKE_BUILD_TYPE} EQUAL "")
	set(HAVE_CMAKE_BUILD_TYPE TRUE)
endif ()
set(CUSTOM_C_FLAGS $ENV{CFLAGS})
set(CUSTOM_CXX_FLAGS $ENV{CXXFLAGS})

set(SOURCE_DIR ${ROOT_DIR}/src)
set(INCLUDE_DIR ${ROOT_DIR}/include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/bin)

if (UNIX AND CMAKE_SYSTEM_NAME MATCHES BSD)
	set(BSD true)
endif ()
if (UNIX AND CMAKE_SYSTEM_NAME MATCHES Linux)
	set(LINUX true)
endif ()

#-----------------------------------------------------------------------
# Find packages and other useful checks
#-----------------------------------------------------------------------

# Look for our custom modules in cmake/Modules:
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

# includes
include(cmake/Macros.cmake)
include(CheckInSourceBuild)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckLibraryExists)

include(cmake/util.cmake)
include(cmake/ReallySmall.cmake)

set(BUILDTYPE_DOCSTRING
	"Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel"
	)
if (WITH_DEBUG OR DEVELOPMENT_BUILD)
	if (HAVE_CMAKE_BUILD_TYPE)
		if (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
			message(WARNING "WITH_DEBUG and DEVELOPMENT_BUILD are in conflict with CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}, disabling Debug/Dev")
			set(WITH_DEBUG FALSE)
			set(DEVELOPMENT_BUILD FALSE)
		elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
			message(WARNING "WITH_DEBUG or DEVELOPMENT_BUILD set to TRUE are in conflict with CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}, updating CMAKE_BUILD_TYPE to RelWithDebInfo")
			set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
		endif ()
	else ()
		set(CMAKE_BUILD_TYPE "Debug" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	endif ()
elseif (NOT HAVE_CMAKE_BUILD_TYPE)
	if (CUSTOM_C_FLAGS)
		set(CMAKE_BUILD_TYPE "" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	elseif (CMAKE_BUILD_TYPE MATCHES "Debug" OR NOT HAVE_CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING ${BUILDTYPE_DOCSTRING} FORCE)
	endif ()
endif ()

include(cmake/CommonCompilerFlags.cmake)

if (FORCE32)
	set(TARGET_M "-m32")
	set(OBJ_FORMAT "elf32-i386")
	set(LINKER_SCRIPT "ld_script_i386.xc")
endif ()

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
# OpenSSL library (for Blowfish)
hunter_add_package(OpenSSL)
find_package(OpenSSL REQUIRED)

set(BoostMods program_options system filesystem thread date_time random log)
if (WITH_TEST)
	list(APPEND BoostMods unit_test_framework)
endif ()
#set(Boost_DEBUG ON)
hunter_add_package(Boost COMPONENTS ${BoostMods})
find_package(Boost CONFIG REQUIRED COMPONENTS ${BoostMods})

# DB
set(WITH_DATABASE OFF)
if (ENABLE_MYSQL)
	find_package(MySQL)
	if (MYSQL_FOUND)
		set(WITH_DATABASE TRUE)
		set(WITH_MYSQL TRUE)
	endif ()
endif ()

check_function_exists(fork HAVE_FORK)

# Generate config.h
configure_file(
	${ROOT_DIR}/cmake/config.h.in
	${INCLUDE_DIR}/config.h
	)
set_source_files_properties(${INCLUDE_DIR}/config.h PROPERTIES GENERATED 1)
list(APPEND ADDITIONAL_MAKE_CLEAN_FILES ${INCLUDE_DIR}/config.h)

# Doxygen is option
set(WITH_DOXYGEN FALSE)
if (ENABLE_DOXYGEN)
	find_package(Doxygen)
	if (DOXYGEN_DOT_FOUND)
		set(WITH_DOXYGEN TRUE)
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
		endif ()
		get_filename_component(DOXYGEN_DOT_EXE "${DOXYGEN_DOT_EXECUTABLE}" DIRECTORY)
		file(TO_NATIVE_PATH "${DOXYGEN_DOT_EXE}" DOXYGEN_DOT_PATH)
		set (DOXY_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
		message(STATUS "Generating ${DOXY_CONFIG}")
		configure_file(
			"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in"
			${DOXY_CONFIG} @ONLY
			)
		add_custom_target(docs
			${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG}
			DEPENDS ${DOXY_CONFIG}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM
			)
	endif ()
endif ()

if (ENABLE_STRIP OR CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
	if (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
		message(WARNING "ENABLE_STRIP & CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} are in conflict, ENABLE_STRIP ignored")
	elseif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
	endif ()
	# TODO: Add support for other compilers
endif ()

if (ENABLE_MULTIBUILD)
	if (WIN32 AND MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		message(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
	endif ()
endif ()

set(GENERATED_INCLUDES)
set(GENERATED_SOURCES)

include_directories(include)
set(SUBDIRS
	gsrc
	src
	)
foreach (subdir ${SUBDIRS})
	add_subdirectory(${subdir})
endforeach ()

message("${PROJECT_NAME} ${PROJECT_VERSION}\n")
if (WIN32 AND MSVC)
	message("Platform: Windows (MSVC)")
elseif (WIN32 AND MINGW)
	message("Platform: Windows (MinGW)")
elseif (WIN32)
	message("Platform: Windows (Other)")
elseif (APPLE)
	message("Platform: Mac")
elseif (BSD)
	message("Platform: BSD")
elseif (LINUX)
	message("Platform: Linux")
else ()
	message("Platform: Other/No platform code")
endif ()
message("System  : ${CMAKE_SYSTEM}")
message("	compiler | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
	cxxflags | ${CMAKE_CXX_FLAGS}
	builddir | ${CMAKE_BINARY_DIR}
	buildtype| ${CMAKE_BUILD_TYPE}
Building with support for:")
show_end_message_yesno("MySQL" WITH_MYSQL)
show_end_message_yesno("Doxygen" WITH_DOXYGEN)
show_end_message_yesno("Debug" WITH_DEBUG)

cable_add_buildinfo_library(PROJECT_NAME Lotospp)
