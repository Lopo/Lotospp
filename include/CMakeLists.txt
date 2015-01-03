set(lotos2_HEADERS
	${CMAKE_CURRENT_BINARY_DIR}/version.h

	AutoID.h
	AutoList.h
	Creature.h
	Exception.h
	Logger.h
	Scheduler.h
	Singleton.h
	Talker.h
	Thing.h
	User.h
	enum.h
	globals.h
	misc.h
	server.h
	system.h
	tasks.h
	)

install(FILES ${lotos2_HEADERS} DESTINATION "${lotos2_INCLUDE_DIR}/lotos2")

add_subdirectory(structs)
add_subdirectory(network)
add_subdirectory(database)
