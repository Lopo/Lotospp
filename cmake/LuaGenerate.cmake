if (__lua_generate)
	return()
endif()
set(__lua_generate YES)

hunter_add_package(Lua)
find_package(Lua CONFIG REQUIRED)

function(LuaGenerate _src generated_h generated_cpp)
	if (NOT ${_src} MATCHES "([^/]+)\\.lua")
		message(ERROR "Invalid filename ${_src} to LuaGenerate")
		return()
	endif()
	set(_file ${CMAKE_MATCH_1})

	file(GLOB_RECURSE deps CONFIGURE_DEPENDS ${ROOT_DIR}/Lua/*.lua)
	set(OUTPUT_H ${CMAKE_CURRENT_BINARY_DIR}/${_file}.h)
	set(OUTPUT_CPP ${CMAKE_CURRENT_BINARY_DIR}/${_file}.cpp)
	add_custom_command(
		OUTPUT ${OUTPUT_H} ${OUTPUT_CPP}
		COMMAND ${LUA_EXECUTABLE} -e "package.path=package.path..';${ROOT_DIR}/Lua/?.lua'" -l Enum ${_src} ${CMAKE_CURRENT_BINARY_DIR}
		MAIN_DEPENDENCY ${_src}
		DEPENDS ${deps}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM
		)
	set_source_files_properties(${OUTPUT_H} ${OUTPUT_CPP}
		DIRECTORY ${ROOT_DIR}
		PROPERTIES
			GENERATED 1
		)

	set(${generated_h} ${OUTPUT_H} PARENT_SCOPE)
	set(${generated_cpp} ${OUTPUT_CPP} PARENT_SCOPE)
endfunction()
