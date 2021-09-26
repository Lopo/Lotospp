add_subdirectory(Enums)

set(SOURCES
	Creature.cpp
	Dispatcher.cpp
	ExceptionHandler.cpp
	IOUser.cpp
	Scheduler.cpp
	Talker.cpp
	Thing.cpp
	User.cpp
	)
source_group(Common FILES ${SOURCES})

add_library(Common ${SOURCES})
add_dependencies(Common CommonEnums)
target_link_libraries(Common
	PRIVATE
		Lotospp-buildinfo
		Commands
		Strings
		Security
		)
if (WITH_DATABASE)
	target_link_libraries(Common
		PRIVATE
			Database
		)
endif()
set_target_properties(Common PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
