set(LuaExec_FIND_QUIETLY true)

set(_POSSIBLE_LUA_EXECUTABLE lua)
set(_POSSIBLE_SUFFIXES "" "53" "5.3" "-5.3" "52" "5.2" "-5.2")

foreach (_SUFFIX ${_POSSIBLE_SUFFIXES})
  list(APPEND _POSSIBLE_LUA_EXECUTABLE "lua${_SUFFIX}${CMAKE_EXECUTABLE_SUFFIX}")
endforeach(_SUFFIX)

find_program(LUAEXEC
	NAMES ${_POSSIBLE_LUA_EXECUTABLE}
	PATHS "${LUAEXEC_ROOT_DIR}"
	PATH_SUFFIXES bin
	)

execute_process(COMMAND
	${LUAEXEC} -v
	OUTPUT_VARIABLE _LUAEXEC_VER
	ERROR_QUIET
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)
if (_LUAEXEC_VER)
	set(LUAEXEC_VERSION_FULL "${_LUAEXEC_VER}")
	string(REGEX
		MATCH "^Lua (5[.0-9]+)[ \\t]"
		tmp
		"${_LUAEXEC_VER}"
		)
	set(LUAEXEC_VERSION "${CMAKE_MATCH_1}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LuaExec DEFAULT_MSG LUAEXEC)

mark_as_advanced(LUAEXEC LUAEXEC_VERSION)
