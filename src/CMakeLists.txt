# Generate config.h
configure_file(
	${ROOT_DIR}/cmake/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/config.h PROPERTIES GENERATED 1)
list(APPEND ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}) # enum.h

set(SUBDIRS
	Common
	Commands
	Log
	Network
	Security
	Strings
	)
foreach (subdir ${SUBDIRS})
	add_subdirectory(${subdir})
endforeach ()
if (WITH_DATABASE)
	add_subdirectory(Database)
endif()

# To find public headers
include_directories(
	BEFORE
	${Boost_INCLUDE_DIR}
#	${OPENSSL_INCLUDE_DIR}
	)

check_function_exists(fork HAVE_FORK)
add_compile_definitions($<$<BOOL:${HAVE_FORK}>:HAVE_FORK>)

set(EXECUTABLE ${PROJECT_NAME})
add_executable(${EXECUTABLE} main.cpp)
make_small_executable(${EXECUTABLE})

#if (WITH_MYSQL)
#	include_directories(${MYSQL_INCLUDE_DIR})
#	link_directories(${MYSQL_LIB_DIR})
#	target_link_libraries(${EXECUTABLE} ${MYSQL_LIB} ${MYSQL_LIBRARIES})
#endif ()
if (UNIX)
	find_package(Threads)
	target_link_libraries(${EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})
endif ()

target_link_libraries(${EXECUTABLE}
	Lotospp-buildinfo
	Common
	CommonEnums
	Log
	Commands
	Network
	Security
	Strings
	${CMAKE_DL_LIBS}
	${SOCKET_LIBRARIES}
	${EXEC_LIBRARIES}
	${COMPAT_LIBRARIES}
	${Boost_LIBRARIES}
	${CRYPTO_LIBRARIES}
	${MISC_LIBRARIES}
	)
if (WITH_DATABASE)
	target_link_libraries(${EXECUTABLE} Database)
endif()
