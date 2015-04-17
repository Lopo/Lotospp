set(lotos2_SRCS
	Creature.cpp
	Exception.cpp
	Dispatcher.cpp
	Scheduler.cpp
	Talker.cpp
	Thing.cpp
	User.cpp

	main.cpp
	misc.cpp

	log/Logger.cpp
	log/severity_t.cpp

	network/Connection.cpp
	network/ConnectionManager.cpp
	network/NetworkMessage.cpp
	network/OutputMessage.cpp
	network/Protocol.cpp

	network/ServicePort.cpp
	network/ServiceManager.cpp

	network/protocol/Telnet.cpp
	)
if (USE_DATABASE)
	set(lotos2_SRCS
		${lotos2_SRCS}

		database/Driver.cpp
		database/Insert.cpp
		database/Query.cpp
		)
endif (USE_DATABASE)
if (MYSQL_FOUND)
	set(lotos2_SRCS
		${lotos2_SRCS}

		database/driver/MySQL.cpp
		)
endif (MYSQL_FOUND)

# To find public headers
include_directories(
	BEFORE
	${INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	)

add_executable(${PROJECT_NAME} ${lotos2_SRCS})

if (MYSQL_FOUND)
	include_directories(${MYSQL_INCLUDE_DIR})
	link_directories(${MYSQL_LIB_DIR})
#	target_link_libraries(${PROJECT_NAME} ${MYSQL_CLIENT_LIBS})
	target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES})
endif (MYSQL_FOUND)

if (UNIX)
	find_package(Threads)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif (UNIX)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} ${SOCKET_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${EXEC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${COMPAT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
