foreach (gsrc ${generated_SRCS})
	set_source_files_properties(${gsrc} PROPERTIES GENERATED 1)
endforeach ()
foreach (ghdr ${generated_HDRS})
	set_source_files_properties(${ghdr} PROPERTIES GENERATED 1)
endforeach ()

set(main_SRCS
	Creature.cpp
	ExceptionHandler.cpp
	Dispatcher.cpp
	Scheduler.cpp
	Talker.cpp
	Thing.cpp
	User.cpp
	IOUser.cpp
	)

set(log_SRCS
	log/Logger.cpp
	log/severity_t.cpp
	)
source_group(log FILES ${log_SRCS})

set(network_SRCS
	network/Connection.cpp
	network/ConnectionManager.cpp
	network/NetworkMessage.cpp
	network/OutputMessage.cpp
	network/Protocol.cpp

	network/ServicePort.cpp
	network/ServiceManager.cpp
	)
source_group(network FILES ${network_SRCS})
set(network_protocol_SRCS
	network/protocol/Telnet.cpp
	)
source_group(network\\protocol FILES ${network_protocol_SRCS})
list(APPEND network_SRCS
	${network_protocol_SRCS}
	)

set(strings_SRCS
	strings/stringPrintf.cpp
	strings/stringSplit.cpp
	strings/misc.cpp
	strings/Splitline.cpp
	)
source_group(strings FILES ${strings_SRCS})

set(security_SRCS
	security/Blowfish.cpp
	)
source_group(security FILES ${security_SRCS})

set(command_SRCS
	command/Say.cpp
	command/Quit.cpp
	)
source_group(command FILES ${command_SRCS})

set(database_SRCS)
if (WITH_DATABASE)
	set(database_SRCS
		database/Driver.cpp
		database/Insert.cpp
		database/Query.cpp
		)
	source_group(database FILES ${database_SRCS})

	if (WITH_MYSQL)
		set(database_driver_SRCS
			${database_driver_SRCS}
			database/driver/MySQL.cpp
			)
	endif ()

	source_group(database\\driver FILES ${database_driver_SRCS})

	list(APPEND database_SRCS
		${database_driver_SRCS}
		)
endif ()

source_group(main FILES ${main_SRCS})

set(lotospp_SRCS
	${generated_SRCS}
	${main_SRCS}
	${log_SRCS}
	${network_SRCS}
	${database_SRCS}
	${strings_SRCS}
	${security_SRCS}
	${command_SRCS}
	)

# To find public headers
include_directories(
	BEFORE
	${INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${OPENSSL_INCLUDE_DIR}
	)

add_library(Lotospp ${lotospp_SRCS})
target_link_libraries(Lotospp Lotospp-buildinfo)
add_executable(${PROJECT_NAME} main.cpp)
make_small_executable(${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} generated_objects)

if (WITH_MYSQL)
	include_directories(${MYSQL_INCLUDE_DIR})
	link_directories(${MYSQL_LIB_DIR})
#	target_link_libraries(${PROJECT_NAME} ${MYSQL_CLIENT_LIBS})
	target_link_libraries(${PROJECT_NAME} ${MYSQL_LIB} ${MYSQL_LIBRARIES})
endif ()

if (UNIX)
	find_package(Threads)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif ()

target_link_libraries(${PROJECT_NAME} Lotospp)

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} ${SOCKET_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${EXEC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${COMPAT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${CRYPTO_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${MISC_LIBRARIES})
