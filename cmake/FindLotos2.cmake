# - Try to find Lotos2
#
# Variables defined:
#   Lotos2_FOUND        - TRUE if Lotos2 was found
#   Lotos2_INCLUDE_DIRS - Lotos2's include directories, not cached
#
#   Lotos2_LOCALE_DIR   - Directory where Lotos2's locales are installed
#   Lotos2_PLUGIN_DIR   - Directory where plugins should be installed
#   Lotos2_SHARE_DIR    - Directory where sounds, icons etc. are installed
#
#   Lotos2_CMAKE_DIR    - Directory where Lotos2's cmake files can be found
#
# Macros defined:
#   LOTOS2_ADD_PLUGIN(plugin_name sources...)
#

# Directory this file is in
get_filename_component(Lotos2_CMAKE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

# Check if Lotos2 is installed
include(${Lotos2_CMAKE_DIR}/Lotos2Paths.cmake OPTIONAL RESULT_VARIABLE _lotos2_installed)

if (_lotos2_installed)
	if (NOT Lotos2_FIND_QUIETLY)
		message(STATUS "Found Lotos2 headers: ${Lotos2_INCLUDE_DIR}")
	endif (NOT Lotos2_FIND_QUIETLY)

	set(Lotos2_FOUND TRUE)
	set(Lotos2_INCLUDE_DIRS
		"${Lotos2_INCLUDE_DIR}"
		"${Lotos2_INCLUDE_DIR}/lotos2"
		)
else (_lotos2_installed)
	# Lotos2 not installed, check if this is the source
	set(LOTOS2_VERSION_SOURCE_DIR "${Lotos2_CMAKE_DIR}/..")
	include(${Lotos2_CMAKE_DIR}/Version.cmake OPTIONAL RESULT_VARIABLE _lotos2_version)

	if (_lotos2_version)
		include(${Lotos2_CMAKE_DIR}/DefaultPaths.cmake)
		get_filename_component(_lotos2_src_dir "${Lotos2_CMAKE_DIR}/.." ABSOLUTE)

		if (NOT Lotos2_FIND_QUIETLY)
			message(STATUS "Found Lotos2 source: ${_lotos2_src_dir}")
		endif (NOT Lotos2_FIND_QUIETLY)

		set(Lotos2_FOUND TRUE)
		set(Lotos2_INCLUDE_DIRS
			"${_lotos2_src_dir}/include"
			"${CMAKE_CURRENT_BINARY_DIR}/include")
	endif (_lotos2_version)
endif (_lotos2_installed)

if (NOT Lotos2_FOUND)
	if (Lotos2_FIND_REQUIRED)
		message(FATAL_ERROR "Lotos2 NOT FOUND")
	endif (Lotos2_FIND_REQUIRED)
endif (NOT Lotos2_FOUND)

# Add common (for daemon and plugin) compiler flags
include(${Lotos2_CMAKE_DIR}/CommonCompilerFlags.cmake)

# make uninstall
if (NOT lotos2_target_prefix)
	configure_file(
		"${Lotos2_CMAKE_DIR}/cmake_uninstall.cmake.in"
		"${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
		@ONLY)
	add_custom_target(
		uninstall
		"${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
endif (NOT lotos2_target_prefix)

# These symbols must be exported from the plugin
#set(_lotos2_plugin_symbols Lotos2GeneralPluginData Lotos2ProtocolPluginData)

#macro (LOTOS2_ADD_PLUGIN _lotos2_plugin_name)
#	add_library(${_lotos2_plugin_name} MODULE ${ARGN})
#	set_target_properties(${_lotos2_plugin_name} PROPERTIES PREFIX "")

	# Link with thread library
#	target_link_libraries(${_lotos2_plugin_name} ${CMAKE_THREAD_LIBS_INIT})

#	if (APPLE)
#		set(_link_flags "-flat_namespace -undefined suppress")

		# Write the list of symbols that should be exported from the plugin to a
		# file and tell the linker about it. One symbol per line with a '_' prefix.
#		string(REPLACE ";" "\n_" _symbols "${_lotos2_plugin_symbols}")
#		set(_symbols_list "${CMAKE_CURRENT_BINARY_DIR}/symbols.list")
#		file(WRITE ${_symbols_list} "_${_symbols}\n")
#		set(_link_flags
#			"${_link_flags} -Wl,-exported_symbols_list,'${_symbols_list}'")

#	elseif (CMAKE_COMPILER_IS_GNUCXX)
		# Create a version script exporting the symbols that should be exported
		# from the plugin and tell the linker about it.
#		set(_symbols "{ global: ${_lotos2_plugin_symbols}; local: *; };")
#		set(_version_script "${CMAKE_CURRENT_BINARY_DIR}/version.script")
#		file(WRITE ${_version_script} "${_symbols}\n")

		# Check if the linker supports version script (i.e. is GNU ld)
#		check_cxx_accepts_flag("-Wl,--version-script,${_version_script}"
#			LD_ACCEPTS_VERSION_SCRIPT)
#		if (LD_ACCEPTS_VERSION_SCRIPT)
#			set(_link_flags "-Wl,--version-script,'${_version_script}'")
#		endif (LD_ACCEPTS_VERSION_SCRIPT)
#	endif (APPLE)

#	if (_link_flags)
#		set_target_properties(${_lotos2_plugin_name} PROPERTIES
#			LINK_FLAGS ${_link_flags}
#			)
#	endif (_link_flags)

#	install(TARGETS ${_lotos2_plugin_name} DESTINATION ${Lotos2_PLUGIN_DIR})
#endmacro (LOTOS2_ADD_PLUGIN)

#macro (_lotos2_plugin_version_helper type)
#	if (DEFINED PLUGIN_VERSION_${type})
#		set(_PLUGIN_VERSION_${type} ${ARGV1}${PLUGIN_VERSION_${type}}${ARGV2})
#	else (DEFINED PLUGIN_VERSION_${type})
#		set(_PLUGIN_VERSION_${type} LOTOS2_VERSION_${type})
#	endif (DEFINED PLUGIN_VERSION_${type})
#endmacro (_lotos2_plugin_version_helper)

#macro (LOTOS2_CREATE_PLUGIN_VERSION_FILE dir)
	# Use PLUGIN_VERSION_x if set, otherwise use LOTOS2_VERSION_x
#	_lotos2_plugin_version_helper(MAJOR)
#	_lotos2_plugin_version_helper(MINOR)
#	_lotos2_plugin_version_helper(PATCH)
#	_lotos2_plugin_version_helper(STATE \" \")

	# When building from a git clone, set the extra version to the HEAD revision,
	# replacing any existing value.
#	find_program(lotos2_git git)
#	if (lotos2_git)
#		execute_process(COMMAND ${lotos2_git} rev-parse HEAD
#			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#			RESULT_VARIABLE lotos2_git_result
#			OUTPUT_VARIABLE lotos2_git_output
#			ERROR_QUIET
#			OUTPUT_STRIP_TRAILING_WHITESPACE)
#		if (${lotos2_git_result} EQUAL 0)
#			string(SUBSTRING ${lotos2_git_output} 0 7 lotos2_git_short)
#			set(_PLUGIN_VERSION_EXTRA "\"-${lotos2_git_short}\"")
#		endif (${lotos2_git_result} EQUAL 0)
#	endif (lotos2_git)

	# pluginversion.h content
#	set(_plugin_version_file "${dir}/pluginversion.h")
#	set(_plugin_old_version "")
#	set(_plugin_new_version 
#		"// Autogenerated by CMake. Do not edit, changes will be lost.
#ifndef LOTOS2_PLUGINVERSION_H
#define LOTOS2_PLUGINVERSION_H

#include <lotos2/lotos2version.h>

#define _LOTOS2_PLUGIN_STR_(x) #x
#define _LOTOS2_PLUGIN_STR(x) _LOTOS2_PLUGIN_STR_(x)

#define PLUGIN_VERSION_MAJOR ${_PLUGIN_VERSION_MAJOR}
#define PLUGIN_VERSION_MINOR ${_PLUGIN_VERSION_MINOR}
#define PLUGIN_VERSION_RELEASE ${_PLUGIN_VERSION_RELEASE}
#define PLUGIN_VERSION_EXTRA ${_PLUGIN_VERSION_EXTRA}
#define PLUGIN_VERSION_STRING \\
#	_LOTOS2_PLUGIN_STR(${_PLUGIN_VERSION_MAJOR}) \".\" \\
#	_LOTOS2_PLUGIN_STR(${_PLUGIN_VERSION_MINOR}) \".\" \\
#	_LOTOS2_PLUGIN_STR(${_PLUGIN_VERSION_RELEASE}) \\
#	${_PLUGIN_VERSION_EXTRA}

#endif
#")

	# Read in the old file (if it exists)
#	if (EXISTS ${_plugin_version_file})
#		file(READ ${_plugin_version_file} _plugin_old_version)
#	endif (EXISTS ${_plugin_version_file})

	# Only write the file if the content has changed
#	string(COMPARE NOTEQUAL
#		"${_plugin_old_version}" "${_plugin_new_version}"
#		_plugin_update_version_file)

#	if (${_plugin_update_version_file})
#		message(STATUS "Creating ${_plugin_version_file}")
#		file(WRITE ${_plugin_version_file} ${_plugin_new_version})
#	endif (${_plugin_update_version_file})
#endmacro (LOTOS2_CREATE_PLUGIN_VERSION_FILE)
