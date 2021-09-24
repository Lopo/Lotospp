set (SOURCES
	Quit.cpp
	Say.cpp
	)
source_group(Commands FILES ${SOURCES})

add_library(Commands ${SOURCES})
add_dependencies(Commands
	CommonEnumsGens
	)
set_target_properties(Commands PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
#target_link_libraries(Commands PRIVATE Lotospp-buildinfo generated)
#target_link_libraries(Commands PRIVATE Boost::log)
