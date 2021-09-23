include(LuaGenerate)

set(SOURCES_TO_GENERATE
	AsciiChar.lua
	LoginCom.lua
	PrintCode.lua
	ReturnValue.lua
	TelnetCmd.lua
	TelnetFlag.lua
	TelnetOpt.lua
	TelnetSub.lua
	UserFlag.lua
	UserLevel.lua
	UserStage.lua
	)

foreach (_file ${SOURCES_TO_GENERATE})
	LuaGenerate(${_file} gh gcpp)
	list(APPEND GENERATED_H ${gh})
	list(APPEND GENERATED_CPP ${gcpp})
endforeach()

add_custom_target(CommonEnumsGens
	DEPENDS ${GENERATED_CPP}
	)

add_library(CommonEnums ${GENERATED_CPP})
set_target_properties(CommonEnums PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
target_include_directories(CommonEnums PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
