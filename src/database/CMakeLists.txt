if (WITH_DATABASE)
#	message(STATUS "generating DB Makefile")
set (SOURCES
	Driver.cpp
	Insert.cpp
	Query.cpp
	)
source_group(Database FILES ${SOURCES})
if (WITH_MYSQL)
	set(DRIVER_SOURCES
		driver/MySQL.cpp
		)
endif()
source_group(Database\\Driver FILES ${DRIVER_SOURCES})
list(APPEND SOURCES
	${DRIVER_SOURCES}
	)

add_library(Database ${SOURCES})
set_target_properties(Database PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
if (WITH_MYSQL)
	target_link_libraries(Database PRIVATE ${MYSQL_LIBRARIES})
endif()
endif(WITH_DATABASE)
