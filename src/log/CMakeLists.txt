set (SOURCES
	Logger.cpp
	severity_t.cpp
	)
source_group(Log FILES ${SOURCES})

add_library(Log ${SOURCES})
target_link_libraries(Log PRIVATE Boost::log)
#target_link_libraries(Log PRIVATE Boost::log Boost::log_setup Boost::thread rt)
set_target_properties(Log PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
