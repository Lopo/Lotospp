set(SOURCES
	misc.cpp
	Splitline.cpp
	stringPrintf.cpp
	stringSplit.cpp
	)
source_group(Strings FILES ${SOURCES})

add_library(Strings ${SOURCES})
set_target_properties(Strings PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
add_dependencies(Strings
	CommonEnumsGens
	)
