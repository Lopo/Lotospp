# Generates include/version.h

# Lotos++ version
set(LOTOSPP_VERSION_STRING "${LOTOSPP_VERSION_MAJOR}.${LOTOSPP_VERSION_MINOR}.${LOTOSPP_VERSION_PATCH}")
set(LOTOSPP_VERSION_BUILD "" CACHE STRING "Stuff to append to version string")

if (LOTOSPP_VERSION_BUILD)
	set(LOTOSPP_VERSION_STRING "${LOTOSPP_VERSION_STRING}-${LOTOSPP_VERSION_BUILD}")
elseif (DEVELOPMENT_BUILD)
	set(LOTOSPP_VERSION_STRING "${LOTOSPP_VERSION_STRING}-dev")
else ()
	# When building from a git clone, set the extra version to the HEAD revision, replacing any existing value
	find_program(lotospp_git git)
	if (lotospp_git)
		if (NOT LOTOSPP_VERSION_SOURCE_DIR)
			set(LOTOSPP_VERSION_SOURCE_DIR ${PROJECT_SOURCE_DIR})
		endif ()

		execute_process(COMMAND ${lotospp_git} rev-parse HEAD
			WORKING_DIRECTORY ${LOTOSPP_VERSION_SOURCE_DIR}
			RESULT_VARIABLE lotospp_git_result
			OUTPUT_VARIABLE lotospp_git_output
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
			)
		if (${lotospp_git_result} EQUAL 0)
			string(SUBSTRING ${lotospp_git_output} 0 7 lotospp_git_short)
			set(LOTOSPP_VERSION_BUILD "-${lotospp_git_short}")
		endif ()
	endif ()
endif ()

set(PROJECT_VERSION_MAJOR ${LOTOSPP_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${LOTOSPP_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${LOTOSPP_VERSION_PATCH})
set(PROJECT_VERSION_TWEAK ${LOTOSPP_VERSION_BUILD})
set(PROJECT_VERSION ${LOTOSPP_VERSION_STRING})

# version.h content
set(lotospp_version_file "${INCLUDE_DIR}/version.h")
set(lotospp_old_version "")
set(lotospp_new_version
	"// ${AUTOGEN_WARNING}
#ifndef LOTOSPP_VERSION_H
#define LOTOSPP_VERSION_H

#define LOTOSPP_VERSION_MAJOR ${LOTOSPP_VERSION_MAJOR}
#define LOTOSPP_VERSION_MINOR ${LOTOSPP_VERSION_MINOR}
#define LOTOSPP_VERSION_PATCH ${LOTOSPP_VERSION_PATCH}
#define LOTOSPP_VERSION_BUILD \"${LOTOSPP_VERSION_BUILD}\"
#define LOTOSPP_VERSION_STRING \"${LOTOSPP_VERSION_STRING}\"

#endif // LOTOSPP_VERSION_H
")

# Read in the old file (if it exists)
if (EXISTS ${lotospp_version_file})
	file(READ ${lotospp_version_file} lotospp_old_version)
endif ()

# Only write the file if the content has changed
string(COMPARE NOTEQUAL
	"${lotospp_old_version}" "${lotospp_new_version}"
	lotospp_update_version_file
	)
if (${lotospp_update_version_file})
	message(STATUS "Creating ${lotospp_version_file}")
	file(WRITE ${lotospp_version_file} ${lotospp_new_version})
endif ()
