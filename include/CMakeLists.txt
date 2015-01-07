set(lotos2_HEADERS
	${CMAKE_CURRENT_BINARY_DIR}/version.h

	AutoID.h
	AutoList.h
	Creature.h
	Exception.h
	Dispatcher.h
	Logger.h
	Scheduler.h
	Singleton.h
	Talker.h
	Task.h
	Thing.h
	User.h
	enum.h
	globals.h
	misc.h
	system.h
	)

install(FILES ${lotos2_HEADERS} DESTINATION "${lotos2_INCLUDE_DIR}/lotos2")

add_subdirectory(structs)
add_subdirectory(network)
add_subdirectory(database)
