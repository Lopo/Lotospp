include(FindLuaExec)
if (NOT LUAEXEC)
	message(FATAL_ERROR "Lua binary not found")
endif ()
message(STATUS "Found Lua binary: ${LUAEXEC} ver ${LUAEXEC_VERSION}")

set(SOURCES_TO_GENERATE
	consts
	enums
	)

file(GLOB_RECURSE deps ${ROOT_DIR}/tools/*.lua)
foreach (_file ${SOURCES_TO_GENERATE})
	add_custom_command(
		OUTPUT ${INCLUDE_DIR}/generated/${_file}.h ${SOURCE_DIR}/generated/${_file}.cpp
		COMMAND ${LUAEXEC} -l tools/enum ${CMAKE_CURRENT_SOURCE_DIR}/${_file}.lua
		MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${_file}.lua
		DEPENDS ${deps}
		WORKING_DIRECTORY ${ROOT_DIR}
		VERBATIM
		)
	list(APPEND GENERATED_INCLUDES ${INCLUDE_DIR}/generated/${_file}.h)
	list(APPEND GENERATED_SOURCES ${SOURCE_DIR}/generated/${_file}.cpp)
endforeach()
set(GENERATED_INCLUDES ${GENERATED_INCLUDES} PARENT_SCOPE)
set(GENERATED_SOURCES ${GENERATED_SOURCES} PARENT_SCOPE)

add_custom_target(generated_objects DEPENDS ${GENERATED_SOURCES})

set_target_properties(generated_objects PROPERTIES
	EXCLUDE_FROM_ALL false
	)
