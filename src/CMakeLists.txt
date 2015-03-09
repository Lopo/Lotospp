set(lotos2_SRCS
	Creature.cpp
	Exception.cpp
	Dispatcher.cpp
	Logger.cpp
	Scheduler.cpp
	Talker.cpp
	Thing.cpp
	User.cpp

	main.cpp
	misc.cpp

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
		database/Driver.cpp
		database/Insert.cpp
		database/Query.cpp

		${lotos2_SRCS}
		)
endif (USE_DATABASE)
if (MYSQL_FOUND)
	set(lotos2_SRCS
		database/driver/MySQL.cpp

		${lotos2_SRCS}
		)
endif (MYSQL_FOUND)

# To find public headers
include_directories(
	BEFORE
	${PROJECT_SOURCE_DIR}/include
	${Boost_INCLUDE_DIR}
	)
if (MYSQL_FOUND)
	include_directories(${MYSQL_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${MYSQL_CLIENT_LIBS})
#	target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES})
endif (MYSQL_FOUND)

add_executable(${PROJECT_NAME} ${lotos2_SRCS})

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} ${SOCKET_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
