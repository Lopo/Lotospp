add_subdirectory(Enums)

add_compile_definitions(MIN_USERNAME_LEN=${MIN_USERNAME_LEN})
add_compile_definitions(MAX_USERNAME_LEN=${MAX_USERNAME_LEN})
add_compile_definitions(MIN_PASSWORD_LEN=${MIN_PASSWORD_LEN})
add_compile_definitions(MAX_LOGIN_ATTEMPTS=${MAX_LOGIN_ATTEMPTS})

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
