foreach (gsrc ${GENERATED_SOURCES})
	set_source_files_properties(${gsrc} PROPERTIES GENERATED 1)
endforeach()
foreach (gh ${GENERATED_INCLUDES})
	set_source_files_properties(${gh} PROPERTIES GENERATED 1)
endforeach()

set(lotospp_SRCS
	${GENERATED_SOURCES}

	Creature.cpp
	Exception.cpp
	Dispatcher.cpp
	Scheduler.cpp
	Talker.cpp
	Thing.cpp
	User.cpp
	IOUser.cpp

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

	strings/stringPrintf.cpp
	strings/stringSplit.cpp

	security/crypt_blowfish.cpp

	command/Say.cpp
	command/Quit.cpp
	)
if (USE_DATABASE)
	set(lotospp_SRCS
		${lotospp_SRCS}

		database/Driver.cpp
		database/Insert.cpp
		database/Query.cpp
		)
endif ()
if (MYSQL_FOUND)
	set(lotospp_SRCS
		${lotospp_SRCS}

		database/driver/MySQL.cpp
		)
endif ()

# To find public headers
include_directories(
	BEFORE
	${INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	)

add_executable(${PROJECT_NAME} ${lotospp_SRCS})
add_dependencies(${PROJECT_NAME} generated_objects)

if (MYSQL_FOUND)
	include_directories(${MYSQL_INCLUDE_DIR})
	link_directories(${MYSQL_LIB_DIR})
#	target_link_libraries(${PROJECT_NAME} ${MYSQL_CLIENT_LIBS})
	target_link_libraries(${PROJECT_NAME} ${MYSQL_LIBRARIES})
endif ()

if (UNIX)
	find_package(Threads)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif ()
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} ${SOCKET_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${EXEC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${COMPAT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
